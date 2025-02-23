#include "Type.h"

namespace Gleam
{
    Type& Type::GetType(const std::type_index typeIndex)
    {
        if (allTypes.contains(typeIndex) == false)
            allTypes.insert({typeIndex, Type{}});
        return allTypes.at(typeIndex);
    }
    std::optional<std::reference_wrapper<Type>> Type::GetType(const std::string_view uuidStr)
    {
        auto uuid = uuids::uuid::from_string(uuidStr);
        if (uuid.has_value() == false)
            throw std::runtime_error("无法识别的UUID字符串！");
        if (uuidToTypeIndex.contains(uuid.value()) == false)
            return std::nullopt;
        return GetType(uuidToTypeIndex.at(uuid.value()));
    }

    uuids::uuid& Type::Uuid()
    {
        return uuid;
    }
    const type_info*& Type::TypeInfo()
    {
        return typeInfo;
    }
    size_t& Type::Size()
    {
        return size;
    }
    Type::TypeConstruct& Type::Construct()
    {
        return construct;
    }
    Type::TypeSerialize& Type::Serialize()
    {
        return serialize;
    }
    bool Type::IsInitialized() const
    {
        return typeInfo != nullptr;
    }
    std::vector<FieldInfo>& Type::FieldInfos()
    {
        return fieldInfos;
    }
    std::optional<std::type_index>& Type::Parent()
    {
        return parent;
    }
}