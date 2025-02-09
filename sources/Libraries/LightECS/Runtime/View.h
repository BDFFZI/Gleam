#pragma once
#include "World.h"
#include "LightMath/Runtime/LinearAlgebra/Matrix.h"

namespace Light
{
    template <class TFunction, class... TComponents>
    concept ViewIterator = requires(TFunction function, TComponents&... components) { function(components...); };

    template <class TFunction, class... TComponents>
    concept ViewIteratorWithEntity = requires(TFunction function, Entity& entity, TComponents&... components) { function(entity, components...); };

    template <class TViewFilter>
    concept ViewFilter = requires(Archetype* archetype, bool isMatched)
    {
        isMatched = TViewFilter::IsMatched(archetype);
    };
    class ViewAlways
    {
    public:
        static bool IsMatched(Archetype* archetype)
        {
            return true;
        }
    };
    static_assert(ViewFilter<ViewAlways>);
    template <Component... TComponents>
    class ViewNecessary
    {
    public:
        static bool IsMatched(Archetype* archetype)
        {
            std::type_index components[] = {typeid(TComponents)...};
            for (size_t i = 0; i < sizeof...(TComponents); ++i)
                if (archetype->HasComponent(components[i]) == false)
                    return false;
            return true;
        }
    };
    static_assert(ViewFilter<ViewNecessary<>>);
    template <Component... TComponents>
    class ViewExclusion
    {
    public:
        static bool IsMatched(Archetype* archetype)
        {
            std::type_index components[] = {typeid(TComponents)...};
            for (size_t i = 0; i < sizeof...(TComponents); ++i)
                if (archetype->HasComponent(components[i]))
                    return false;
            return true;
        }
    };
    static_assert(ViewFilter<ViewExclusion<>>);

    template <class...>
    class View
    {
    };

    /**
     * @brief 针对实体堆的检视工具
     * 
     * 实体堆 <code> std::vector<Heap> </code> 是一种根据原形顺序生成的实体容器。默认情况下实体及其组件是以字节序列的形式存放在实体堆中，所以并不利于读写。
     * 利用\c View 则可自动识别所需组件的存储位置，并将其转换成组件引用的形式供使用者遍历，从而方便的对实体的批量处理。
     * 
     * @tparam TComponents 
     */
    template <ViewFilter TFilter,Component... TComponents>
        requires (sizeof...(TComponents) != 0)
    class View<TFilter, TComponents...>
    {
    public:
        template <class TFunction> requires
            ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        static void Each(TFunction function)
        {
            Query();
            Each_Inner(function, std::make_index_sequence<sizeof...(TComponents)>());
        }
        static int Count()
        {
            Query();
            return Count_Inner();
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
                for (auto& archetype : Archetype::GetAllArchetypes())
                {
                    if (ViewNecessary<TComponents...>::IsMatched(archetype.get())
                        && TFilter::IsMatched(archetype.get()))
                    {
                        targetArchetypes.emplace_back(archetype.get());
                        targetComponentOffsets.emplace_back(archetype->MemoryMap<TComponents...>());
                    }
                }

                targetArchetypeCount = static_cast<int>(targetArchetypes.size());
                isQueried = true;
            }
        }

        template <class TFunction, size_t... Indices>
            requires ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        static void Each_Inner(TFunction function, std::index_sequence<Indices...>)
        {
            for (int i = 0; i < targetArchetypeCount; i++)
            {
                const std::array<int, sizeof...(TComponents)>& componentOffset = targetComponentOffsets[i];
                World::GetEntityHeap(targetArchetypes[i]).ForeachElements([function,componentOffset](std::byte* item)
                {
                    if constexpr (ViewIterator<TFunction, TComponents...>)
                        function(*reinterpret_cast<TComponents*>(item + componentOffset[Indices])...);
                    else if constexpr (ViewIteratorWithEntity<TFunction, TComponents...>)
                        function(*reinterpret_cast<Entity*>(item), *reinterpret_cast<TComponents*>(item + componentOffset[Indices])...);
                });
            }
        }
        static int Count_Inner()
        {
            int count = 0;
            for (int i = 0; i < targetArchetypeCount; i++)
                count += World::GetEntityHeap(targetArchetypes[i]).GetCount();
            return count;
        }
    };

    template <Component TComponent,Component... TComponents>
        requires (sizeof...(TComponents) + 1 != 0) && !ViewFilter<TComponent>
    class View<TComponent, TComponents...> : public View<ViewAlways, TComponent, TComponents...>
    {
    };
}