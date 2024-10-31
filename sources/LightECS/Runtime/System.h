#pragma once
#include <vector>
#include <optional>
#include <concepts>
#include "Archetype.h"
#include "Heap.h"
#include "World.h"


template <class... TComponents>
    requires (sizeof...(TComponents) != 0)
// requires name<TComponents...>
class View
{
public:
    View(ArchetypeMetaInstance<TComponents...>&)
    {
    }
    View()
    {
        if (isQueried == false)
        {
            int count = static_cast<int>(Archetype::allArchetypes.size());
            for (int i = 0; i < count; i++)
            {
                Archetype& archetype = Archetype::allArchetypes[i];
                if (archetype.Contains<TComponents...>())
                {
                    archetypeIDs.emplace_back(i);
                    componentOffsets.emplace_back(archetype.MemoryMap<TComponents...>());
                }
            }
            archetypeCount = static_cast<int>(archetypeIDs.size());
            isQueried = true;
        }
    }

    void Each(World& world, const std::function<void(TComponents&...)>& function) const
    {
        for (int i = 0; i < archetypeCount; i++)
        {
            const int archetypeID = archetypeIDs[i];
            const std::array<int, sizeof...(TComponents)>& componentOffset = componentOffsets[i];
            Heap& heap = world.GetEntityHeaps()[archetypeID];
            heap.ForeachElements([function,componentOffset](std::byte* item)
            {
                int component = -1;
                function(*reinterpret_cast<TComponents*>(item + componentOffset[++component])...);
            });
        }
    }

private:
    inline static bool isQueried = false;
    inline static std::vector<int> archetypeIDs = {};
    inline static std::vector<std::array<int, sizeof...(TComponents)>> componentOffsets = {};
    inline static int archetypeCount = {};
};

class System
{
public:
    virtual ~System() = default;
    virtual void Update(World& world) const = 0;
};
