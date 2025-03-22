#pragma once
#include <typeindex>
#include <vector>

#include "Archetype.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
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
            std::type_index components[] = {typeid(TComponents)...};
            for (size_t i = 0; i < sizeof...(TComponents); ++i)
                if (archetype.HasComponent(components[i]) == false)
                    return false;
            return true;
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

    class Query
    {
    public:
        template <QueryFilter TFilter,Component... TComponents> requires (sizeof...(TComponents) != 0)
        static Query& GetQuery()
        {
            //利用静态局部变量初始化的特性，仅在首次查询时创建并计算目标
            static Query query = []
            {
                Query query;

                //创建目标检测函数
                query.addArchetypes = [&query](const Archetype& archetype)
                {
                    if (QueryNecessary<TComponents...>::IsMatched(archetype) //包含目标组件
                        && TFilter::IsMatched(archetype)) //满足自定义筛选器
                    {
                        query.targets.emplace_back(
                            &archetype, std::vector{archetype.GetComponentOffset(typeid(TComponents))...}
                        );
                    }
                };
                //查询已有实体
                for (auto& archetype : Archetype::GetAllArchetypes() | UnwrapRef)
                    query.addArchetypes(archetype);
                //注册到组件关联查询
                (componentToQueries[typeid(TComponents)].emplace_back(&query), ...);

                return query;
            }();

            return query;
        }

        Query(Query&) = delete;
        Query& operator=(Query&) = delete;
        Query(Query&&) = default;
        Query& operator=(Query&&) = default;

        const std::vector<std::tuple<const Archetype*, std::vector<int>>>& GetTargets()
        {
            return targets;
        }

    private:
        friend class Archetype;

        inline static std::unordered_map<std::type_index, std::vector<Query*>> componentToQueries = {};

        std::vector<std::tuple<const Archetype*, std::vector<int>>> targets = {};
        std::function<void(const Archetype&)> addArchetypes;

        Query() = default;
    };
}