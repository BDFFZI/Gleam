#pragma once
#include <assert.h>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <stduuid/uuid.h>

#include "Transferrer/MemberTransferrer.h"
#include "Transferrer/Transferrer.h"

namespace Light
{
    template <typename T>
    struct Type_Construct
    {
        static void Invoke(void* address)
        {
            new(address) T();
        }
    };
    template <typename T,Transferrer TTransferrer>
    struct Type_Transfer
    {
        // 若要支持成员序列化功能，需特化下方函数
        // static void Invoke(TTransferrer& transferrer, T& value)
        // {
        // }
    };
    class DataTransferrer;
    class Type
    {
        using Construct = void (*)(void* address);
        using Serialize = void (*)(DataTransferrer&, void*);

    public:
        template <typename T>
        static Type* Register(const std::string_view uuidStr, const std::optional<std::type_index> parent = std::nullopt)
        {
            assert(parent.has_value() ? GetType(parent.value()) != nullptr:true && "父类未注册类型信息！");

            std::unique_ptr<Type>& type = allTypes.emplace_back(new Type());

            uuids::uuid uuid = uuids::uuid::from_string(uuidStr).value_or(uuids::uuid());
            uuidToType.insert({uuid, type.get()});
            indexToType.insert({typeid(T), type.get()});

            type->parent = parent;
            type->uuid = uuid;
            type->info = &typeid(T);
            type->size = sizeof(T);
            type->construct = Type_Construct<T>::Invoke;

            //根据传输器获取序列化函数
            if constexpr (requires() { Type_Transfer<T, DataTransferrer>::Invoke; }) //有自定义序列化函数
                type->serialize = [](DataTransferrer& serializer, void* ptr)
                {
                    Type_Transfer<T, DataTransferrer>::Invoke(serializer, *static_cast<T*>(ptr));
                };
            else if (parent.has_value()) //无自定义序列化函数，但有父类序列化函数
            {
                Type* parentType = GetType(parent.value());
                type->serialize = parentType->serialize;
            }
            else //兜底使用空序列化函数
            {
                type->serialize = [](DataTransferrer&, void*)
                {
                };
            }

            //利用传输器获取成员信息
            if constexpr (requires() { Type_Transfer<T, MemberTransferrer>::Invoke; })
            {
                std::byte instance[sizeof(T)];
                type->construct(instance); //构造一个实例
                MemberTransferrer memberTransferrer = {instance};
                Type_Transfer<T, MemberTransferrer>::Invoke(memberTransferrer, *reinterpret_cast<T*>(instance));
                type->fieldInfos.swap(memberTransferrer.GetResult()); //获取成员信息
            }


            return type.get();
        }
        static Type* GetType(const std::string_view uuidStr)
        {
            uuids::uuid uuid = uuids::uuid::from_string(uuidStr).value();
            return uuidToType.at(uuid);
        }
        static Type* GetType(const std::type_index name)
        {
            return indexToType.contains(name) ? indexToType.at(name) : nullptr;
        }

        uuids::uuid uuid;
        const type_info* info = nullptr;
        size_t size = 0;
        Construct construct = nullptr;
        Serialize serialize = nullptr;
        std::vector<FieldInfo> fieldInfos;
        std::optional<std::type_index> parent;

    private:
        inline static std::vector<std::unique_ptr<Type>> allTypes = {};
        inline static std::unordered_map<uuids::uuid, Type*> uuidToType = {};
        inline static std::unordered_map<std::type_index, Type*> indexToType = {};
    };
}

#include "Transferrer/DataTransferrer.h"

#define Light_MakeType_Friend \
template <typename T,Transferrer TTransferrer>\
friend struct ::Light::Type_Transfer;
#define Light_MakeType_AddField(field)\
transferrer.TransferField(#field, value.field)
#define Light_MakeType(type,uuidStr,...)\
inline const ::Light::Type* type##Type = ::Light::Type::Register<type>(uuidStr,__VA_ARGS__);\
template <Light::Transferrer TTransferrer>\
struct ::Light::Type_Transfer<type, TTransferrer>\
{static void Invoke(TTransferrer& transferrer, type& value);};\
template <Light::Transferrer TTransferrer>\
void ::Light::Type_Transfer<type,TTransferrer>::Invoke(TTransferrer& transferrer, type& value)