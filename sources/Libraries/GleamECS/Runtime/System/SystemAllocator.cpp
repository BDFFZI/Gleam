#include "SystemAllocator.h"

namespace Gleam
{
    bool SystemDataComparer::operator()(const SystemData* left, const SystemData* right) const
    {
        if (left->order == right->order)
            return left < right;
        return left->order < right->order;
    }

    void SystemAllocator::AddSystem(SystemInfo& systemInfo)
    {
        if (systemInfo.group != nullptr)
            AddSystem(*systemInfo.group);

        auto& [system,count] = systems[&systemInfo];
        if (++count == 1) //首次添加
        {
            system.type = systemInfo.type;
            system.order = systemInfo.order;
            if (systemInfo.update.index() == 0)
                system.update = std::get<0>(systemInfo.update);
            else
            {
                std::function updateGroup = [&subSystems = system.subSystems](EntityAllocator& entities)
                {
                    for (auto subSystem : subSystems)
                    {
                        subSystem->update(entities);
                    }
                };
                system.update = [systemGroupUpdate = std::get<1>(systemInfo.update),updateGroup](EntityAllocator& entities)
                {
                    systemGroupUpdate(entities, updateGroup);
                };
            }

            if (systemInfo.group != nullptr)
            {
                SystemData& parentSystem = std::get<0>(systems[&systemInfo]);
                parentSystem.subSystems.insert(&system);
            }
            else
            {
                topSystems.insert(&system);
            }
        }
    }
    void SystemAllocator::RemoveSystem(SystemInfo& systemInfo)
    {
        if (systemInfo.group != nullptr)
            RemoveSystem(*systemInfo.group);

        auto& [system,count] = systems.at(&systemInfo);
        if (--count == 0) //最终移除
        {
            //从组移除
            if (systemInfo.group != nullptr)
            {
                SystemData& parentSystem = std::get<0>(systems[&systemInfo]);
                parentSystem.subSystems.erase(&system);
            }
            //销毁实例
            systems.erase(&systemInfo);
        }
    }
    void SystemAllocator::Update(EntityAllocator& entityAllocator) const
    {
        for (auto subSystem : topSystems)
            subSystem->update(entityAllocator);
    }
}