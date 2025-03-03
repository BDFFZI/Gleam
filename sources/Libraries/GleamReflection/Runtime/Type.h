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
        static void Invoke(void* source, void* destination)
        {
            *static_cast<T*>(destination) = std::move(*static_cast<T*>(source));
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

    /**
     * 存储运行时类型信息
     * 注意点1：使用uuid访问Type时，Type始终存在，只是默认情况下各项值都为空。
     * 注意点2：此处的类型是C++语言内置类型的超集，意味着你可以伪造在C++中实际不存在的类型，因此该Type的标识符是用户自定的uuid而不是type_index。
     * 但你依然可以用C++内置的RTTI快速创建Type，并利用type_index间接访问。
     */
    class Type
    {
    public:
        /**
         * 基于模板、type_info创建Type，并建立type_index到Type的索引
         * @tparam T 
         * @param typeID 
         * @param parent 
         * @return 
         */
        template <typename T>
        static Type& Create(const uuids::uuid typeID, const std::optional<std::reference_wrapper<const Type>> parent = std::nullopt)
        {
            typeIndexToID.insert({typeid(T), typeID}); //建立利用type_index间接访问的索引

            assert(!allTypes.contains(typeID) && "已有相同编号的类型！");
            Type& type = allTypes.insert({typeID, Type{}}).first->second;
            type.id = typeID;
            type.size = sizeof(T);
            type.parent = parent;
            type.construct = Type_Raii<T>::Construct;
            type.destruct = Type_Raii<T>::Destruct;
            type.move = Type_Move<T>::Invoke;

            //利用传输器获取成员字段信息
            {
                std::byte instance[sizeof(T)];
                type.construct(instance); //构造一个实例
                MemberTransferrer memberTransferrer = {instance};
                Type_Transfer<T, MemberTransferrer>::Invoke(memberTransferrer, *reinterpret_cast<T*>(instance));
                type.fields.swap(memberTransferrer.GetResult()); //获取成员信息
            }

            if (parent.has_value())
            {
                static const Type& parentType = parent.value();
                //父类字段
                type.fields.insert(type.fields.begin(), parentType.fields.begin(), parentType.fields.end());
                //含父类的序列化
                type.serialize = [](DataTransferrer& serializer, void* ptr)
                {
                    parentType.serialize(serializer, ptr);
                    Type_Transfer<T, DataTransferrer>::Invoke(serializer, *static_cast<T*>(ptr));
                };
            }
            else
            {
                //序列化
                type.serialize = [](DataTransferrer& serializer, void* ptr)
                {
                    Type_Transfer<T, DataTransferrer>::Invoke(serializer, *static_cast<T*>(ptr));
                };
            }

            return type;
        }
        /**
         * 利用多个Type组合来伪造一个新Type，用于实现原型序列化
         * @param components 
         * @return 
         */
        static Type& Create(std::initializer_list<std::reference_wrapper<const Type>> components);

        static std::optional<std::reference_wrapper<Type>> GetType(uuids::uuid typeID);
        static std::optional<std::reference_wrapper<Type>> GetType(std::type_index typeIndex);

        uuids::uuid GetID() const;
        size_t GetSize() const;
        std::optional<std::reference_wrapper<const Type>> GetParent() const;
        const std::vector<FieldInfo>& GetFields() const;
        const std::function<void(void*)>& GetConstruct() const;
        const std::function<void(void*)>& GetDestruct() const;
        const std::function<void(void*, void*)>& GetMove() const;
        const std::function<void(DataTransferrer&, void*)>& GetSerialize() const;

    private:
        inline static std::unordered_map<uuids::uuid, Type> allTypes; //unordered_map扩容不会修改元素地址
        inline static std::unordered_map<std::type_index, uuids::uuid> typeIndexToID = {};

        uuids::uuid id = {};
        size_t size = 0;
        std::optional<std::reference_wrapper<const Type>> parent = std::nullopt;
        std::vector<FieldInfo> fields = {};
        std::function<void(void*)> construct = nullptr;
        std::function<void(void*)> destruct = nullptr;
        std::function<void(void*, void*)> move = nullptr;
        std::function<void(DataTransferrer&, void*)> serialize = nullptr;
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
inline const ::Gleam::Type& type##Type = ::Gleam::Type::Create<type>(\
uuids::uuid::from_string(uuidStr).value(),\
parent,\
__VA_ARGS__);\
template <Gleam::Transferrer TTransferrer>\
struct ::Gleam::Type_Transfer<type, TTransferrer>\
{\
static void Invoke(TTransferrer& transferrer, type& value);\
};\
template <Gleam::Transferrer TTransferrer>\
void ::Gleam::Type_Transfer<type,TTransferrer>::Invoke(TTransferrer& transferrer, type& value)

#define Gleam_MakeType_AddField(field)\
transferrer.TransferField(#field, value.field)

#define Gleam_MakeType(type,uuidStr) Gleam_MakeType_Inner(type,uuidStr,std::nullopt)
#define Gleam_MakeType2(type,uuidStr,parent) Gleam_MakeType_Inner(type,uuidStr,parent)

#define Gleam_MakeCombinedType(typeName,...)\
inline const ::Gleam::Type& typeName = ::Gleam::Type::Create({__VA_ARGS__});