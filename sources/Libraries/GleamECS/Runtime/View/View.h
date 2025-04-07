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
        View() = default;
        View(EntityAllocator& entityAllocator): entityAllocator(&entityAllocator)
        {
        }

        template <class TFunction> requires
            ViewIterator<TFunction, TComponents...> || ViewIteratorWithEntity<TFunction, TComponents...>
        void Each(TFunction function)
        {
            static Query& query = Query::GetQuery<TFilter, TComponents...>();

            constexpr static auto Each_Inner = []<size_t... Indices>(EntityAllocator& entityAllocator, TFunction function, std::index_sequence<Indices...>)
            {
                for (const auto& [archetype,componentOffsets] : query.GetTargets())
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

            Each_Inner(*entityAllocator, function, std::make_index_sequence<sizeof...(TComponents)>());
        }
        void Fetch(std::vector<Entity>& result)
        {
            static Query& query = Query::GetQuery<TFilter, TComponents...>();

            for (const auto& [archetype,componentOffsets] : query.GetTargets())
            {
                Heap& heap = entityAllocator->GetEntityHeap(*archetype);
                heap.ForeachElements([&result](std::byte* item)
                {
                    result.push_back(*reinterpret_cast<Entity*>(item));
                });
            }
        }
        int Count() const
        {
            static Query& query = Query::GetQuery<TFilter, TComponents...>();

            int count = 0;

            for (const auto& [archetype,componentOffsets] : query.GetTargets())
            {
                count += entityAllocator->GetEntityHeap(*archetype).GetCount();
            }

            return count;
        }

    private:
        EntityAllocator* entityAllocator;
    };

    template <Component TComponent,Component... TComponents>
        requires (sizeof...(TComponents) + 1 != 0) && !QueryFilter<TComponent>
    class View<TComponent, TComponents...> : public View<QueryAlways, TComponent, TComponents...>
    {
    public:
        View() = default;
        View(EntityAllocator& entityAllocator): View<QueryAlways, TComponent, TComponents...>(entityAllocator)
        {
        }
    };
}