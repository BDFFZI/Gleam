#pragma once
#include <memory>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <uuid.h>

#include "Serialization/Serializer.hpp"

namespace Light
{
    template <typename T>
    struct TypeConstruct
    {
        static void Invoke(void* address)
        {
            new(address) T();
        }
    };

    template <class TTransferrer>
    concept Transferable = requires(TTransferrer& transferrer, void* value) { transferrer.TransferField("value", value); };

    template <typename T,Transferable TTransferrer>
    struct TypeTransfer
    {
        static void Invoke(TTransferrer& transferrer, T& value)
        {
        }
    };

    struct Type
    {
        using Construct = void (*)(void* address);
        using Serialize = void (*)(Serializer&, void*);

        inline static std::vector<std::unique_ptr<Type>> allTypes = {};
        inline static std::unordered_map<uuids::uuid, Type*> uuidToType = {};
        inline static std::unordered_map<std::type_index, Type*> indexToType = {};

        template <typename T>
        static Type* Register(const std::string_view uuidStr)
        {
            uuids::uuid uuid;
            if (auto result = uuids::uuid::from_string(uuidStr))
                uuid = *result;
            else
                throw std::invalid_argument("UUID字符串无效！");

            std::unique_ptr<Type>& type = allTypes.emplace_back(new Type());
            uuidToType.insert({uuid, type.get()});
            indexToType.insert({typeid(T), type.get()});

            type->uuid = uuid;
            type->info = &typeid(T);
            type->size = sizeof(T);
            type->construct = TypeConstruct<T>::Invoke;
            type->serialize = [](Serializer& serializer, void* ptr) { TypeTransfer<T, Serializer>::Invoke(serializer, *static_cast<T*>(ptr)); };
            type->deserialize = type->serialize;
            return type.get();
        }

        uuids::uuid uuid;
        const type_info* info = nullptr;
        size_t size = 0;
        Construct construct = nullptr;
        Serialize serialize = nullptr;
        Serialize deserialize = nullptr;
        std::vector<int> fieldOffsets;
        std::vector<std::type_index> fieldTypes;
    };

#define MakeType_AddField(field) transferrer.TransferField(#field, value.field)
#define MakeType(uuidStr,type,...)\
    Light::Type* type##Type = Light::Type::Register<type>(uuidStr);\
    template <Transferable TTransferrer>\
    struct Light::TypeTransfer<type, TTransferrer>\
    {static void Invoke(TTransferrer& transferrer, type& value);};\
    template <Transferable TTransferrer>\
    void Light::TypeTransfer<type,TTransferrer>::Invoke(TTransferrer& transferrer, type& value)
}
