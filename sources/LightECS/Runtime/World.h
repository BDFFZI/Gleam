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
    template <typename... TComponent>
    void ForeachEntities(const int archetypeID, std::function<void(TComponent&...)> iterator)
    {
        archetypeHeaps[archetypeID].ForeachElements([](const int itemIndex, const std::byte* item)
        {
        });
    }

private:
    std::vector<Heap> archetypeHeaps;
};
