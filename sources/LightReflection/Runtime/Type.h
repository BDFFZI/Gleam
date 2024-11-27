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
    template <class Serializer>
    concept TSerializer = requires(Serializer& serializer, void* value) { serializer.TransferField("value", value); };

    template <typename T>
    struct TypeOperator
    {
        static void Construct(void* address)
        {
            new(address) T();
        }
        template <TSerializer Serializer>
        static void Serialize(Serializer& serializer, T& value)
        {
        }
    };

    struct Type
    {
        using Constructor = void (*)(void* address);
        using Serializer = void (*)(Serializer&, void*);

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

            std::unique_ptr<Type>& type = allTypes.emplace_back();
            uuidToType.insert({uuid, type.get()});
            indexToType.insert({typeid(T), type.get()});

            type->uuid = uuid;
            type->info = typeid(T);
            type->size = sizeof(T);
            type->constructor = TypeOperator<T>::Construct;
            type->serializer = [](Serializer& transferrer, void* ptr) { TypeOperator<T>::Serialize(transferrer, *static_cast<T*>(ptr)); };
            type->deserializer = type->serializer;
            return type.get();
        }

        uuids::uuid uuid;
        std::type_info* info = nullptr;
        size_t size = 0;
        Constructor constructor = nullptr;
        Serializer serializer = nullptr;
        Serializer deserializer = nullptr;
        std::vector<int> fieldOffsets;
        std::vector<std::type_index> fieldTypes;
    };

#define MakeType_AddField(field) serializer.TransferField(#field, value.field)
#define MakeType(uuidStr,type,...)\
Light::Type* type##Type = Light::Type::Register<type>(uuidStr);\
template <class TSerializer>\
void TypeOperator<type>::Serialize(TSerializer& serializer, type& value)
}
