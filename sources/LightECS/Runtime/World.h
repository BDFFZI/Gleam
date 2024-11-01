#pragma once
#include "Archetype.h"
#include "Heap.h"

class World
{
public:
    World()
    {
        for (const auto& archetype : Archetype::allArchetypes)
            entityHeaps.emplace_back(archetype->GetSize());
    }

    std::vector<Heap>& GetEntityHeaps() { return entityHeaps; }

    void AddEntity(const int archetypeID)
    {
        entityHeaps[archetypeID].AddElement([archetypeID](std::byte* item)
        {
            Archetype::allArchetypes[archetypeID]->RunConstructor(item);
        });
    }
    void AddEntities(const int archetypeID, const int count)
    {
        entityHeaps[archetypeID].AddElements(count, [archetypeID](int itemIndex, std::byte* item)
        {
            Archetype::allArchetypes[archetypeID]->RunConstructor(item);
        });
    }

private:
    std::vector<Heap> entityHeaps;
};
