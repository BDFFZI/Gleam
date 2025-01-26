#pragma once
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <stduuid/uuid.h>

#include "Transferrer/DataTransferrer.h"
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
        static void Invoke(TTransferrer& transferrer, T& value)
        {
            throw std::logic_error("未实现类型传输函数！");
        }
    };
    class Type
    {
        using Construct = void (*)(void* address);
        using Serialize = void (*)(DataTransferrer&, void*);

    public:
        template <typename T>
        static Type* Register(const std::string_view uuidStr)
        {
            std::unique_ptr<Type>& type = allTypes.emplace_back(new Type());

            uuids::uuid uuid = uuids::uuid::from_string(uuidStr).value_or(uuids::uuid());
            uuidToType.insert({uuid, type.get()});
            indexToType.insert({typeid(T), type.get()});

            type->uuid = uuid;
            type->info = &typeid(T);
            type->size = sizeof(T);
            type->construct = Type_Construct<T>::Invoke;
            type->serialize = [](DataTransferrer& serializer, void* ptr) { Type_Transfer<T, DataTransferrer>::Invoke(serializer, *static_cast<T*>(ptr)); };
            type->deserialize = type->serialize;

            std::byte instance[sizeof(T)];
            type->construct(instance);
            MemberTransferrer memberTransferrer = {instance};
            Type_Transfer<T, MemberTransferrer>::Invoke(memberTransferrer, *reinterpret_cast<T*>(instance));
            type->fieldInfos.swap(memberTransferrer.GetResult());

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
        Serialize deserialize = nullptr;
        std::vector<FieldInfo> fieldInfos;

    private:
        inline static std::vector<std::unique_ptr<Type>> allTypes = {};
        inline static std::unordered_map<uuids::uuid, Type*> uuidToType = {};
        inline static std::unordered_map<std::type_index, Type*> indexToType = {};
    };
}

#define Light_MakeType_Friend \
template <typename T,Transferrer TTransferrer>\
friend struct ::Light::Type_Transfer;
#define Light_MakeType_AddField(field)\
transferrer.TransferField(#field, value.field)
#define Light_MakeType(type,uuidStr,...)\
inline const ::Light::Type* type##Type = ::Light::Type::Register<type>(uuidStr);\
template <Light::Transferrer TTransferrer>\
struct ::Light::Type_Transfer<type, TTransferrer>\
{static void Invoke(TTransferrer& transferrer, type& value);};\
template <Light::Transferrer TTransferrer>\
void ::Light::Type_Transfer<type,TTransferrer>::Invoke(TTransferrer& transferrer, type& value)