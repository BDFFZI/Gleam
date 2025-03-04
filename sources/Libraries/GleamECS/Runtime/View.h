#pragma once
#include "World.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
    template <class TViewFilter>
    concept ViewFilter = requires(const Archetype& archetype, bool isMatched)
    {
        isMatched = TViewFilter::IsMatched(archetype);
    };
    class ViewAlways
    {
    public:
        static bool IsMatched(const Archetype&)
        {
            return true;
        }
    };
    static_assert(ViewFilter<ViewAlways>);
    template <Component... TComponents>
    class ViewNecessary
    {
    public:
        static bool IsMatched(const Archetype& archetype)
        {
            std::type_index components[] = {typeid(TComponents)...};
            for (size_t i = 0; i < sizeof...(TComponents); ++i)
                if (archetype.HasComponent(components[i]) == false)
                    return false;
            return true;
        }
    };
    static_assert(ViewFilter<ViewNecessary<>>);
    template <Component... TComponents>
    class ViewExclusion
    {
    public:
        static bool IsMatched(const Archetype& archetype)
        {
            std::type_index components[] = {typeid(TComponents)...};
            for (size_t i = 0; i < sizeof...(TComponents); ++i)
                if (archetype.HasComponent(components[i]))
                    return false;
            return true;
        }
    };
    static_assert(ViewFilter<ViewExclusion<>>);

    /**
     * 视图是一种检视工具，用于检视世界中的所有实体。
     * 实体原始的存储方式是基于字节的，而视图可以将其转化为类型对象，并且支持对象筛选等高级功能。
     */
    template <class...>
    class View
    {
    };

    template <class TFunction, class... TComponents>
    concept ViewIterator = requires(TFunction function, TComponents&... components) { function(components...); };

    template <class TFunction, class... TComponents>
    concept ViewIteratorWithEntity = requires(TFunction function, Entity& entity, TComponents&... components) { function(entity, components...); };

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
        static void Fetch(std::vector<Entity>& result)
        {
            Query();
            for (int i = 0; i < targetArchetypeCount; i++)
            {
                Heap& heap = World::GetEntityHeap(*targetArchetypes[i]);
                heap.ForeachElements([&result](std::byte* item)
                {
                    result.push_back(*reinterpret_cast<Entity*>(item));
                });
            }
        }

    private:
        inline static bool isQueried = false;
        inline static std::vector<const Archetype*> targetArchetypes = {};
        inline static std::vector<std::array<int, sizeof...(TComponents)>> targetComponentOffsets = {};
        inline static int targetArchetypeCount = {};

        /**
         * 查询符合的原型并缓存查询结果以加速后续查询。
         */
        static void Query()
        {
            if (isQueried == false)
            {
                for (auto& archetype : Archetype::GetAllArchetypes() | UnwrapRef)
                {
                    if (ViewNecessary<TComponents...>::IsMatched(archetype)
                        && TFilter::IsMatched(archetype))
                    {
                        targetArchetypes.emplace_back(&archetype);
                        targetComponentOffsets.emplace_back(archetype.GetComponentOffset<TComponents...>());
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
                World::GetEntityHeap(*targetArchetypes[i]).ForeachElements([function,componentOffset](std::byte* item)
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
                count += World::GetEntityHeap(*targetArchetypes[i]).GetCount();
            return count;
        }
    };

    template <Component TComponent,Component... TComponents>
        requires (sizeof...(TComponents) + 1 != 0) && !ViewFilter<TComponent>
    class View<TComponent, TComponents...> : public View<ViewAlways, TComponent, TComponents...>
    {
    };
}