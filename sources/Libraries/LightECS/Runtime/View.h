#pragma once
#include "World.h"
#include "LightMath/Runtime/Matrix.h"

namespace Light
{
    template <class TFunction, class... TComponents>
    concept ViewIterator = requires(TFunction function, TComponents&... components) { function(components...); };

    template <class TFunction, class... TComponents>
    concept ViewIteratorWithEntity = requires(TFunction function, Entity& entity, TComponents&... components) { function(entity, components...); };

    /**
     * @brief 针对实体堆的检视工具
     * 
     * 实体堆 <code> std::vector<Heap> </code> 是一种根据原形顺序生成的实体容器。默认情况下实体及其组件是以字节序列的形式存放在实体堆中，所以并不利于读写。
     * 利用\c View 则可自动识别所需组件的存储位置，并将其转换成组件引用的形式供使用者遍历，从而方便的对实体的批量处理。
     * 
     * @tparam TComponents 
     */
    template <Component... TComponents>
        requires (sizeof...(TComponents) != 0)
    class View
    {
    public:
        template <class TFunction> requires
            ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        static void Each(TFunction function)
        {
            Query();
            Each_Inner(function, std::make_index_sequence<sizeof...(TComponents)>());
        }
        template <class TFunction> requires
            ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        static void Each(Scene& scene, TFunction function)
        {
            Query();
            Each_Inner(scene, function, std::make_index_sequence<sizeof...(TComponents)>());
        }

    private:
        inline static bool isQueried = false;
        inline static std::vector<Archetype*> targetArchetypes = {};
        inline static std::vector<std::array<int, sizeof...(TComponents)>> targetComponentOffsets = {};
        inline static int targetArchetypeCount = {};

        /**
         * 查询符合的原型并缓存查询结果以加速后续查询。
         */
        static void Query()
        {
            if (isQueried == false)
            {
                for (auto& item : Archetype::GetAllArchetypes())
                {
                    Archetype& archetype = *item;
                    if (archetype.IsMatched<TComponents...>())
                    {
                        targetArchetypes.emplace_back(&archetype);
                        targetComponentOffsets.emplace_back(archetype.MemoryMap<TComponents...>());
                    }
                }

                targetArchetypeCount = static_cast<int>(targetArchetypes.size());
                isQueried = true;
            }
        }

        template <class TFunction, size_t... Indices>
            requires ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        static void Each_Inner(Scene& scene, TFunction function, std::index_sequence<Indices...>)
        {
            for (int i = 0; i < targetArchetypeCount; i++)
            {
                const Archetype& archetype = *targetArchetypes[i];
                const std::array<int, sizeof...(TComponents)>& componentOffset = targetComponentOffsets[i];
                scene.GetEntityHeap(archetype).ForeachElements([function,componentOffset](std::byte* item)
                {
                    if constexpr (ViewIterator<TFunction, TComponents...>)
                        function(*reinterpret_cast<TComponents*>(item + componentOffset[Indices])...);
                    else if constexpr (ViewIteratorWithEntity<TFunction, TComponents...>)
                        function(*reinterpret_cast<Entity*>(item), *reinterpret_cast<TComponents*>(item + componentOffset[Indices])...);
                });
            }
        }

        template <class TFunction, size_t... Indices>
            requires ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        static void Each_Inner(TFunction function, std::index_sequence<Indices...>)
        {
            for (auto& element : World::GetAllScenes())
            {
                if (element->GetVisibility())
                    Each_Inner(*element, function, std::index_sequence<Indices...>());
            }
        }
    };
}