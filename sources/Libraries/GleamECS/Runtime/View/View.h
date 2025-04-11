#pragma once
#include "Query.h"
#include "GleamECS/Runtime/Entity/EntityAllocator.h"

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
        static void Each(EntityAllocator& entities, TFunction function)
        {
            constexpr static auto Each_Inner = []<size_t... Indices>(EntityAllocator& entityAllocator, TFunction function, std::index_sequence<Indices...>)
            {
                for (const auto& [archetype,componentOffsets] : Query::GetQuery<TFilter, TComponents...>().GetTargets())
                {
                    entityAllocator.GetEntityHeap(*archetype).ForeachElements([function,&componentOffsets](std::byte* item)
                    {
                        if constexpr (ViewIterator<TFunction, TComponents...>)
                            function(*reinterpret_cast<TComponents*>(item + componentOffsets[Indices])...);
                        else if constexpr (ViewIteratorWithEntity<TFunction, TComponents...>)
                            function(*reinterpret_cast<Entity*>(item), *reinterpret_cast<TComponents*>(item + componentOffsets[Indices])...);
                    });
                }
            };

            Each_Inner(entities, function, std::make_index_sequence<sizeof...(TComponents)>());
        }
        template <class TComponent>
        static TComponent& First(EntityAllocator& entities)
        {
            const Archetype& archetype = *std::get<0>(Query::GetQuery<TFilter, TComponents...>().GetTargets().front());
            std::byte* address = entities.GetEntityHeap(archetype).At(0) + archetype.GetComponentOffset(typeid(TComponent));
            return *reinterpret_cast<TComponent*>(address);
        }
        static void Fetch(EntityAllocator& entities, std::vector<Entity>& result)
        {
            for (const auto& [archetype,componentOffsets] : Query::GetQuery<TFilter, TComponents...>().GetTargets())
            {
                Heap& heap = entities.GetEntityHeap(*archetype);
                heap.ForeachElements([&result](std::byte* item)
                {
                    result.push_back(*reinterpret_cast<Entity*>(item));
                });
            }
        }
        int Count(EntityAllocator& entities) const
        {
            int count = 0;

            for (const auto& [archetype,componentOffsets] : Query::GetQuery<TFilter, TComponents...>().GetTargets())
            {
                count += entities.GetEntityHeap(*archetype).GetCount();
            }

            return count;
        }
    };

    template <Component TComponent,Component... TComponents>
        requires (sizeof...(TComponents) + 1 != 0) && !QueryFilter<TComponent>
    class View<TComponent, TComponents...> : public View<QueryAlways, TComponent, TComponents...>
    {
    };
}