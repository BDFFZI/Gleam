#pragma once
#include "Archetype.h"
#include "Heap.h"
#include "System.h"

class World
{
public:
    World()
    {
        for (const auto& archetype : Archetype::allArchetypes)
            heaps.emplace_back(archetype->GetSize());
    }

    std::vector<Heap>& GetHeaps() { return heaps; }

    void AddEntity(const int archetypeID)
    {
        heaps[archetypeID].AddElement([archetypeID](std::byte* item)
        {
            Archetype::allArchetypes[archetypeID]->RunConstructor(item);
        });
    }
    void AddEntities(const int archetypeID, const int count)
    {
        heaps[archetypeID].AddElements(count, [archetypeID](int itemIndex, std::byte* item)
        {
            Archetype::allArchetypes[archetypeID]->RunConstructor(item);
        });
    }
    void AddSystem(const System& system)
    {
        systems.push_back(&system);
    }

    void Update()
    {
        for (auto system : systems)
            system->Update(heaps);
    }

private:
    std::vector<Heap> heaps;
    std::vector<const System*> systems;
};
