#pragma once
#include "GleamECS/Runtime/Entity/Archetype.h"

namespace Gleam
{
    /**
     * 一种用于检测原型是否满足组件需求的条件判断器
     */
    template <class TViewFilter>
    concept QueryFilter = requires(const Archetype& archetype, bool isMatched)
    {
        isMatched = TViewFilter::IsMatched(archetype);
    };

    class QueryAlways
    {
    public:
        static bool IsMatched(const Archetype&)
        {
            return true;
        }
    };
    static_assert(QueryFilter<QueryAlways>);

    template <Component... TComponents>
    class QueryNecessary
    {
    public:
        static bool IsMatched(const Archetype& archetype)
        {
            if constexpr (sizeof...(TComponents) == 0)
                return true;
            else
            {
                std::type_index components[] = {typeid(TComponents)...};
                for (size_t i = 0; i < sizeof...(TComponents); ++i)
                    if (archetype.HasComponent(components[i]) == false)
                        return false;
                return true;
            }
        }
    };
    static_assert(QueryFilter<QueryNecessary<>>);

    template <Component... TComponents>
    class QueryExclusion
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
    static_assert(QueryFilter<QueryExclusion<>>);
}