#pragma once
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include "uu"

struct Type
{
    inline static std::unordered_map<std::type_index, std::unique_ptr<Type>> allTypes = {};

    template <typename T>
    static std::unique_ptr<Type>& Register()
    {
        allTypes.insert({typeid(T), std::make_unique<Type>()});
        auto& type = allTypes.at(typeid(T));

        type->typeInfo = typeid(T);
        type->size = sizeof(T);
        
        return type;
    }

    uuid
    std::type_info* typeInfo;
    size_t size;
};
