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
        static void MoveConstruct(void* source, void* destination)
        {
            new(destination) T(std::move(*static_cast<T*>(source)));
        }
        static void Move(void* source, void* destination)
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
     * 运行时类型信息。
     * Type是RTTI的增强版，实现了包括RAII，序列化，反射等功能。
     * Type可用uuid访问，uuid是为了解决type_index无法存储的问题。
     */
    class Type
    {
    public:
        /**
         * 基于模板、type_info创建Type，并建立type_index到Type的索引
         * @tparam T 
         * @param id 
         * @param parent 
         * @return 
         */
        template <typename T>
        static Type Create(const uuids::uuid id, const std::optional<std::reference_wrapper<const Type>> parent = std::nullopt)
        {
            assert(!indexToType.contains(typeid(T)) && "类型已创建！");
            assert(!indexToType.contains(typeid(T)) && "编号已占用！");

            Type type = Type{};
            type.index = typeid(T);
            type.id = id;
            type.size = sizeof(T);
            type.parent = parent;
            type.construct = Type_Raii<T>::Construct;
            type.destruct = Type_Raii<T>::Destruct;
            type.moveConstruct = Type_Raii<T>::MoveConstruct;
            type.move = Type_Raii<T>::Move;

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

            //依赖RVO
            indexToType.emplace(type.index, &type);
            idToType.emplace(type.id, &type);

            return type;
        }

        static std::optional<std::reference_wrapper<const Type>> GetType(std::type_index typeIndex);
        static std::optional<std::reference_wrapper<const Type>> GetType(uuids::uuid typeID);

        std::string_view GetName() const;
        std::type_index GetIndex() const;
        uuids::uuid GetID() const;
        int GetSize() const;
        std::optional<std::reference_wrapper<const Type>> GetParent() const;
        const std::vector<FieldInfo>& GetFields() const;

        void Construct(void* address) const;
        void Destruct(void* address) const;
        void MoveConstruct(void* source, void* destination) const;
        void Move(void* source, void* destination) const;
        void Serialize(DataTransferrer& transferrer, void* address) const;

    private:
        inline static std::unordered_map<std::type_index, const Type*> indexToType = {}; //unordered_map扩容不会修改元素地址
        inline static std::unordered_map<uuids::uuid, const Type*> idToType = {};

        std::type_index index = typeid(void);
        uuids::uuid id = {};
        int size = 0;
        std::optional<std::reference_wrapper<const Type>> parent = std::nullopt;
        std::vector<FieldInfo> fields = {};
        std::function<void(void*)> construct = nullptr;
        std::function<void(void*)> destruct = nullptr;
        std::function<void(void*, void*)> moveConstruct = nullptr;
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
inline const ::Gleam::Type type##Type = ::Gleam::Type::Create<type>(\
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