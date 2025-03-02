#include "Type.h"

namespace Gleam
{
    Type& Type::GetType(const std::type_index typeIndex)
    {
        if (allTypes.contains(typeIndex) == false)
            allTypes.insert({typeIndex, Type{}});
        return allTypes.at(typeIndex);
    }
    std::optional<std::reference_wrapper<Type>> Type::GetType(const uuids::uuid uuid)
    {
        if (uuidToTypeIndex.contains(uuid) == false)
            return std::nullopt;
        return GetType(uuidToTypeIndex.at(uuid));
    }
    std::optional<std::reference_wrapper<Type>> Type::GetType(const std::string_view uuidStr)
    {
        auto uuid = uuids::uuid::from_string(uuidStr);
        if (uuid.has_value() == false)
            throw std::runtime_error("无法识别的UUID字符串！");
        return GetType(uuid.value());
    }

    uuids::uuid Type::GetID() const
    {
        return id;
    }
    const type_info* Type::GetTypeInfo() const
    {
        return typeInfo;
    }
    size_t Type::GetSize() const
    {
        return size;
    }
    Type::TypeRaiiFunc Type::GetConstruct() const
    {
        return construct;
    }
    Type::TypeRaiiFunc Type::GetDestruct() const
    {
        return destruct;
    }
    Type::TypeMove Type::GetMove() const
    {
        return move;
    }
    Type::TypeSerialize Type::GetSerialize() const
    {
        return serialize;
    }
    bool Type::IsInitialized() const
    {
        return typeInfo != nullptr;
    }
    const std::vector<FieldInfo>& Type::GetFieldInfos() const
    {
        return fieldInfos;
    }
    std::optional<std::type_index> Type::GetParent() const
    {
        return parent;
    }
}