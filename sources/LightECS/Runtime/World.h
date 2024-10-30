#pragma once
#include "Archetype.h"
#include "Heap.h"

class World
{
public:
    World()
    {
        // archetypeHeaps.reserve(Archetype::allArchetypes.size());
        // for (const auto* archetype : Archetype::allArchetypes)
        //     archetypeHeaps.emplace_back(archetype->GetSize());
    }

    void AddEntities(const int archetypeID, const int count)
    {
        archetypeHeaps[archetypeID].AddElements(count);
    }
    void ForeachEntities(const int archetypeID, const std::function<void(std::byte* item)>& iterator)
    {
        archetypeHeaps[archetypeID].ForeachElements(iterator);
    }

private:
    std::vector<Heap> archetypeHeaps;
};
