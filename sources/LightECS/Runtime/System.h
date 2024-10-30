#pragma once
#include <functional>

#include "World.h"

template <class... TComponents>
struct Query
{
    std::vector<int> archetypeIDs;
    std::vector<int[sizeof...(TComponents)]> componentOffsets;

    Query()
    {
        int count = static_cast<int>(Archetype::allArchetypes.size());
        for (int i = 0; i < count; i++)
        {
            Archetype& archetype = Archetype::allArchetypes[i];
            if (archetype.Contains<TComponents...>())
            {
                archetypeIDs.push_back(i);
                
            }
        }
    }
};


class System
{
public:
    template <class... TComponents>
    void Foreach(const Query<TComponents...>& query, std::function<void(TComponents&...)> iterator)
    {
    }

private:
    const World& world;
};
