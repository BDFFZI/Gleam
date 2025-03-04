#include "Type.h"
#include <ranges>

namespace Gleam
{
    std::optional<std::reference_wrapper<const Type>> Type::GetType(const std::type_index typeIndex)
    {
        if (allTypes.contains(typeIndex) == false)
            return std::nullopt;
        return allTypes.at(typeIndex);
    }
    std::optional<std::reference_wrapper<const Type>> Type::GetType(const uuids::uuid typeID)
    {
        if (idToType.contains(typeID) == false)
            return std::nullopt;
        return *idToType.at(typeID);
    }

    std::string_view Type::GetName() const
    {
        return index.name();
    }
    std::type_index Type::GetIndex() const
    {
        return index;
    }
    uuids::uuid Type::GetID() const
    {
        return id;
    }
    int Type::GetSize() const
    {
        return size;
    }
    std::optional<std::reference_wrapper<const Type>> Type::GetParent() const
    {
        return parent;
    }
    const std::vector<FieldInfo>& Type::GetFields() const
    {
        return fields;
    }

    void Type::SetParent(const std::optional<std::reference_wrapper<const Type>> parent)
    {
        this->parent = parent;
    }

    void Type::Construct(void* address) const
    {
        construct(address);
    }
    void Type::Destruct(void* address) const
    {
        destruct(address);
    }
    void Type::MoveConstruct(void* source, void* destination) const
    {
        moveConstruct(source, destination);
    }
    void Type::Move(void* source, void* destination) const
    {
        move(source, destination);
    }
    void Type::Serialize(FieldDataTransferrer& transferrer, void* address, const bool serializeParent) const
    {
        if (serializeParent && parent.has_value())
            parent.value().get().serialize(transferrer, address);
        serialize(transferrer, address);
    }
}