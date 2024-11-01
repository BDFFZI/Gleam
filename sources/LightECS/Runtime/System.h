#pragma once
#include <vector>
#include "Archetype.h"
#include "Heap.h"
#include "World.h"


template <class... TComponents>
    requires (sizeof...(TComponents) != 0)
class View
{
public:
    View(ArchetypeMeta<TComponents...>&)
    {
    }
    View()
    {
        if (isQueried == false)
        {
            int count = static_cast<int>(Archetype::allArchetypes.size());
            for (int i = 0; i < count; i++)
            {
                const Archetype* archetype = Archetype::allArchetypes[i];
                if (archetype->Contains<TComponents...>())
                {
                    archetypeIDs.emplace_back(i);
                    componentOffsets.emplace_back(archetype->MemoryMap<TComponents...>());
                }
            }
            archetypeCount = static_cast<int>(archetypeIDs.size());
            isQueried = true;
        }
    }


    void Each(World& world, const std::function<void(TComponents&...)>& function) const
    {
        Each_Inner(world, function, std::make_index_sequence<sizeof...(TComponents)>());
    }

private:
    inline static bool isQueried = false;
    inline static std::vector<int> archetypeIDs = {};
    inline static std::vector<std::array<int, sizeof...(TComponents)>> componentOffsets = {};
    inline static int archetypeCount = {};

    template <size_t... Indices>
    static void Each_Inner(World& world, const std::function<void(TComponents&...)>& function, std::index_sequence<Indices...>)
    {
        for (int i = 0; i < archetypeCount; i++)
        {
            const int archetypeID = archetypeIDs[i];
            const std::array<int, sizeof...(TComponents)>& componentOffset = componentOffsets[i];

            Heap& entityHeap = world.GetEntityHeaps()[archetypeID];
            entityHeap.ForeachElements([function,componentOffset](std::byte* item)
            {
                function(*reinterpret_cast<TComponents*>(item + componentOffset[Indices])...);
            });
        }
    }
};

class System
{
public:
    virtual ~System() = default;
    virtual void Update(World& world) const = 0;
};
