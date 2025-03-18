#include "EntityInfoAllocator.h"

namespace Gleam
{
    EntityInfo::EntityInfo(const Archetype& archetype, EntityAllocator& allocator, const int memoryIndex, std::byte* memoryAddress)
        : archetype(&archetype), allocator(&allocator), memoryIndex(memoryIndex), memoryAddress(memoryAddress)
    {
    }

    bool EntityInfoAllocator::HasEntity(const Entity entity) const
    {
        return entityInfos.contains(entity);
    }
    EntityInfo& EntityInfoAllocator::GetEntityInfo(const Entity entity)
    {
        return entityInfos.at(entity);
    }
    Entity EntityInfoAllocator::NextEntity()
    {
        return static_cast<Entity>(nextEntity++);
    }
    void EntityInfoAllocator::SetEntityInfo(const Entity entity, const std::optional<EntityInfo>& info)
    {
        if (info.has_value())
            entityInfos[entity] = info.value();
        else
            entityInfos.erase(entity);
    }
    void EntityInfoAllocator::Clear()
    {
        nextEntity = 1;
        entityInfos.clear();
    }
}