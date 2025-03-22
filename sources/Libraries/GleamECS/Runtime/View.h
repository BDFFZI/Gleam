#pragma once
#include "World/World.h"
#include "Query.h"

namespace Gleam
{
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

    template <QueryFilter TFilter,Component... TComponents>
        requires (sizeof...(TComponents) != 0)
    class View<TFilter, TComponents...>
    {
    public:
        template <class TFunction> requires
            ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        static void Each(TFunction function)
        {
            Each_Inner(function, std::make_index_sequence<sizeof...(TComponents)>());
        }
        static void Fetch(std::vector<Entity>& result)
        {
            Query& query = Query::GetQuery<TFilter, TComponents...>();
            for (const auto& [archetype,componentOffsets] : query.GetTargets())
            {
                Heap& heap = World::GetEntityAllocator().GetEntityHeap(*archetype);
                heap.ForeachElements([&result](std::byte* item)
                {
                    result.push_back(*reinterpret_cast<Entity*>(item));
                });
            }
        }
        static int Count()
        {
            int count = 0;

            Query& query = Query::GetQuery<TFilter, TComponents...>();
            for (const auto& [archetype,componentOffsets] : query.GetTargets())
            {
                count += World::GetEntityAllocator().GetEntityHeap(*archetype).GetCount();
            }

            return count;
        }

    private:
        template <class TFunction, size_t... Indices>
            requires ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        static void Each_Inner(TFunction function, std::index_sequence<Indices...>)
        {
            Query& query = Query::GetQuery<TFilter, TComponents...>();
            for (const auto& [archetype,componentOffsets] : query.GetTargets())
            {
                World::GetEntityAllocator().GetEntityHeap(*archetype).ForeachElements([function,&componentOffsets](std::byte* item)
                {
                    if constexpr (ViewIterator<TFunction, TComponents...>)
                        function(*reinterpret_cast<TComponents*>(item + componentOffsets[Indices])...);
                    else if constexpr (ViewIteratorWithEntity<TFunction, TComponents...>)
                        function(*reinterpret_cast<Entity*>(item), *reinterpret_cast<TComponents*>(item + componentOffsets[Indices])...);
                });
            }
        }
    };

    template <Component TComponent,Component... TComponents>
        requires (sizeof...(TComponents) + 1 != 0) && !QueryFilter<TComponent>
    class View<TComponent, TComponents...> : public View<QueryAlways, TComponent, TComponents...>
    {
    };
}