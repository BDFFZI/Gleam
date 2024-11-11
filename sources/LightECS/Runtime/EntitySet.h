#pragma once
#include "Entity.hpp"
#include "Heap.h"
#include "LightECS/Runtime/Archetype.hpp"

struct EntityInfo
{
    int archetypeID;
    int indexAtHeap;
    std::byte* components;
};

class EntitySet
{
public:
    EntitySet();

    std::vector<Heap>& GetEntityHeaps() { return entityHeaps; }

    void AddEntity(int archetypeID, Entity* outEntity);
    void AddEntities(int archetypeID, int count, Entity* outEntities);
    void MoveEntity(Entity entity, int newArchetypeID);
    void RemoveEntity(Entity entity);

private:
    std::vector<Heap> entityHeaps;
    std::unordered_map<Entity, EntityInfo> entityInfos;
    uint32_t nextEntity = 0;

    void RemoveHeapItem(int archetypeID,int index);
};
