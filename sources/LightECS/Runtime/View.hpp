#pragma once
#include "LightECS/Runtime/EntitySet.h"


/**
 * @brief 针对实体堆的检视工具
 * 
 * 实体堆 <code> std::vector<Heap> </code> 是一种根据原形顺序生成的实体容器。默认情况下实体及其组件是以字节序列的形式存放在实体堆中，所以并不利于读写。
 * 利用\c View 则可自动识别所需组件的存储位置，并将其转换成组件引用的形式供使用者遍历，从而方便的对实体的批量处理。
 * 
 * @tparam TComponents 
 */
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


    void Each(EntitySet& entitySet, const std::function<void(TComponents&...)>& function) const
    {
        Each_Inner(entitySet, function, std::make_index_sequence<sizeof...(TComponents)>());
    }

private:
    inline static bool isQueried = false;
    inline static std::vector<int> archetypeIDs = {};
    inline static std::vector<std::array<int, sizeof...(TComponents)>> componentOffsets = {};
    inline static int archetypeCount = {};

    template <size_t... Indices>
    static void Each_Inner(EntitySet& entitySet, const std::function<void(TComponents&...)>& function, std::index_sequence<Indices...>)
    {
        for (int i = 0; i < archetypeCount; i++)
        {
            const int archetypeID = archetypeIDs[i];
            const std::array<int, sizeof...(TComponents)>& componentOffset = componentOffsets[i];

            Heap& entityHeap = entitySet.GetEntityHeaps()[archetypeID];
            entityHeap.ForeachElements([function,componentOffset](std::byte* item)
            {
                function(*reinterpret_cast<TComponents*>(item + componentOffset[Indices])...);
            });
        }
    }
};
