#pragma once
#include "Heap.h"
#include "LightECS/Runtime/Archetype.hpp"

struct EntityInfo
{
    const Archetype* archetype;
    std::byte* components;
    int indexAtHeap;
};

class EntitySet
{
public:
    Heap* GetEntityHeap(const Archetype& archetype)
    {
        auto iterator = entityHeaps.find(&archetype);
        if (iterator == entityHeaps.end())
            return nullptr;
        return &iterator->second;
    }
    EntityInfo* GetEntityInfo(const Entity entity)
    {
        auto iterator = entityInfos.find(entity);
        if (iterator == entityInfos.end())
            return nullptr;
        return &iterator->second;
    }

    void AddEntity(const Archetype& archetype, Entity* outEntity = nullptr);
    void AddEntities(const Archetype& archetype, int count, Entity* outEntities = nullptr);
    void MoveEntity(Entity entity, const Archetype& newArchetype);
    void RemoveEntity(Entity entity);

private:
    std::unordered_map<const Archetype*, Heap> entityHeaps;
    std::unordered_map<Entity, EntityInfo> entityInfos;
    uint32_t nextEntity = 1;

    Heap& GetOrNewEntityHeap(const Archetype& archetype);
    void RemoveHeapItem(const Archetype& archetype, int index);
};
