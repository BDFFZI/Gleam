#pragma once
#include <typeindex>
#include <vector>

#include "GleamUtility/Runtime/Ranges.h"
#include "QueryFilter.h"

namespace Gleam
{
    /**
     * 查询并缓存满足组件需求的原型
     */
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
                query.addArchetype = [&query](const Archetype& archetype)
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
                    query.addArchetype(archetype);
                //注册后续实体创建事件
                Archetype::AddCreateArchetypeEvent(query.addArchetype);

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
        std::vector<std::tuple<const Archetype*, std::vector<int>>> targets = {};
        std::function<void(const Archetype&)> addArchetype;

        Query() = default;
    };
}