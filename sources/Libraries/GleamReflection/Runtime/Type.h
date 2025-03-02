#pragma once
#include <assert.h>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <stduuid/uuid.h>

#include "Transferrer/MemberTransferrer.h"
#include "Transferrer/Transferrer.h"

namespace Gleam
{
    template <typename T>
    struct Type_Raii
    {
        static void Construct(void* address)
        {
            new(address) T();
        }
        static void Destruct(void* address)
        {
            static_cast<T*>(address)->T::~T();
        }
    };
    template <typename T>
    struct Type_Move
    {
        static void Invoke(void* left, void* right)
        {
            *static_cast<T*>(left) = std::move(*static_cast<T*>(right));
        }
    };
    template <typename T,Transferrer TTransferrer>
    struct Type_Transfer
    {
        static void Invoke(TTransferrer& transferrer, T& value)
        {
        }
    };
    class DataTransferrer;
    class Type
    {
        using TypeRaiiFunc = void (*)(void* address);
        using TypeMove = void (*)(void* left, void* right);
        using TypeSerialize = void (*)(DataTransferrer&, void*);

    public:
        /**
         * 辅助函数，利用模板功能快速设置Type信息。
         * 该函数不是创建Type的必要函数，Type始终存在，只是默认情况下各项值都为空，利用该函数可以快速设置它们。
         * @tparam T 
         * @param uuidStr 
         * @param parent 
         * @return 
         */
        template <typename T>
        static Type& Init(const std::string_view uuidStr, const std::optional<std::type_index> parent = std::nullopt)
        {
            Type& type = GetType(typeid(T));
            uuids::uuid uuid = uuids::uuid::from_string(uuidStr).value_or(type.id);
            if (uuid.is_nil() == false)
                uuidToTypeIndex.insert({uuid, typeid(T)});

            type.id = uuid;
            type.parent = parent;
            type.typeInfo = &typeid(T);
            type.size = sizeof(T);
            type.construct = Type_Raii<T>::Construct;
            type.destruct = Type_Raii<T>::Destruct;
            type.move = Type_Move<T>::Invoke;
            type.serialize = [](DataTransferrer& serializer, void* ptr)
            {
                Type_Transfer<T, DataTransferrer>::Invoke(serializer, *static_cast<T*>(ptr));
            };

            //利用传输器获取成员信息
            if constexpr (requires() { Type_Transfer<T, MemberTransferrer>::Invoke; })
            {
                std::byte instance[sizeof(T)];
                type.construct(instance); //构造一个实例
                MemberTransferrer memberTransferrer = {instance};
                Type_Transfer<T, MemberTransferrer>::Invoke(memberTransferrer, *reinterpret_cast<T*>(instance));
                type.fieldInfos.swap(memberTransferrer.GetResult()); //获取成员信息
            }

            return type;
        }
        static Type& GetType(std::type_index typeIndex);
        static std::optional<std::reference_wrapper<Type>> GetType(uuids::uuid uuid);
        static std::optional<std::reference_wrapper<Type>> GetType(std::string_view uuidStr);

        uuids::uuid GetID() const;
        const type_info* GetTypeInfo() const;
        const std::vector<FieldInfo>& GetFieldInfos() const;
        std::optional<std::type_index> GetParent() const;
        size_t GetSize() const;
        TypeRaiiFunc GetConstruct() const;
        TypeRaiiFunc GetDestruct() const;
        TypeMove GetMove() const;
        TypeSerialize GetSerialize() const;

        bool IsInitialized() const;

    private:
        inline static std::unordered_map<std::type_index, Type> allTypes; //unordered_map扩容不会修改元素地址
        inline static std::unordered_map<uuids::uuid, std::type_index> uuidToTypeIndex = {};

        uuids::uuid id = {};
        const type_info* typeInfo = nullptr;
        std::vector<FieldInfo> fieldInfos = {};
        std::optional<std::type_index> parent = std::nullopt;
        size_t size = 0;
        TypeRaiiFunc construct = nullptr;
        TypeRaiiFunc destruct = nullptr;
        TypeMove move = nullptr;
        TypeSerialize serialize = nullptr;
    };
}

#include "Transferrer/DataTransferrer.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "GleamUtility/Runtime/Macro.h"

#define Gleam_MakeType_Friend \
template <typename T,Transferrer TTransferrer>\
friend struct ::Gleam::Type_Transfer;\
template <typename T>\
friend struct ::Gleam::Type_Raii;

#define Gleam_MakeType_Inner(type,uuidStr,parent,...)\
inline const std::string type##Type##ID = uuidStr;\
inline const ::Gleam::Type& type##Type = ::Gleam::Type::Init<type>(\
uuidStr,\
MacroV(std::is_same_v<MacroT<parent>,void>?std::nullopt:std::optional<std::type_index>(typeid(MacroT<parent>))),\
__VA_ARGS__);\
template <Gleam::Transferrer TTransferrer>\
struct ::Gleam::Type_Transfer<type, TTransferrer>\
{\
using TargetType = type;\
using TargetParentType = MacroT<parent>;\
static void Invoke(TTransferrer& transferrer, type& value);\
};\
template <Gleam::Transferrer TTransferrer>\
void ::Gleam::Type_Transfer<type,TTransferrer>::Invoke(TTransferrer& transferrer, type& value)

#define Gleam_MakeType_AddField(field)\
transferrer.TransferField(#field, value.field)

#define Gleam_MakeType_AddParentField()\
if constexpr (!std::is_same_v<TargetParentType, void>)\
    Type_Transfer<TargetParentType, TTransferrer>::Invoke(transferrer, value);

#define Gleam_MakeType(type,uuidStr) Gleam_MakeType_Inner(type,uuidStr,void)
#define Gleam_MakeType2(type,uuidStr,parent) Gleam_MakeType_Inner(type,uuidStr,parent)