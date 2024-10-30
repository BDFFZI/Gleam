#pragma once
#include <vector>
#include <optional>
#include "Archetype.h"
#include "Heap.h"

template <class... TComponents>
class Process
{
public:
    Process(const std::function<void(TComponents&...)>& function)
        : function(function)
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
    void Update(std::vector<Heap>& archetypeHeaps) const
    {
        for (int i = 0; i < archetypeCount; i++)
        {
            const int archetypeID = archetypeIDs[i];
            const std::array<int, sizeof...(TComponents)>& componentOffset = componentOffsets[i];
            Heap& heap = archetypeHeaps[archetypeID];
            heap.ForeachElements([this,componentOffset](std::byte* item)
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

    std::function<void(TComponents&...)> function;
};

class System
{
public:
    virtual ~System() = default;
    virtual void Update(std::vector<Heap>& archetypeHeaps) const = 0;
};


#define MakeProcess(name, ...)\
Process<auto> name = Process(name##Func);\
static void name##Func(__VA_ARGS__)
