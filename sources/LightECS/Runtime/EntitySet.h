#pragma once
#include "Heap.h"
#include "LightECS/Runtime/Archetype.hpp"

class EntitySet
{
public:
    EntitySet();

    std::vector<Heap>& GetEntityHeaps() { return entityHeaps; }

    void AddEntity(int archetypeID);
    void AddEntities(int archetypeID, int count);

private:
    std::vector<Heap> entityHeaps;
};
