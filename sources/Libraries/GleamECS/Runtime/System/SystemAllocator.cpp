#include "SystemAllocator.h"

namespace Gleam
{
    SystemAllocator::~SystemAllocator()
    {
        if (systems.empty())
            return;

        std::vector<std::tuple<SystemInfo*, int>> usageCount;
        usageCount.reserve(systems.size());
        for (auto& system : systems)
            usageCount.emplace_back(system.first, std::get<1>(system.second));

        std::ranges::sort(usageCount, [](std::tuple<SystemInfo*, int>& left, std::tuple<SystemInfo*, int>& right)
        {
            return std::get<1>(left) < std::get<1>(right);
        });

        for (auto& [system,count] : usageCount)
        {
            if (systems.contains(system))
                RemoveSystem(*system);
        }
    }

    IOrderedSystemEvent& SystemAllocator::AddSystem(SystemInfo& systemInfo)
    {
        if (systemInfo.group != nullptr)
            AddSystem(*systemInfo.group);

        auto& [system,count] = systems[&systemInfo];
        if (++count == 1) //首次添加
        {
            system = static_cast<IOrderedSystemEvent*>(systemInfo.type->Create());
            system->Start();

            ISystemGroup& systemGroup = systemInfo.group ? *dynamic_cast<ISystemGroup*>(std::get<0>(systems[systemInfo.group])) : rootSystem;
            systemGroup.AddSystem(*system);
        }
        return *system;
    }
    void SystemAllocator::RemoveSystem(SystemInfo& systemInfo)
    {
        auto& [system,count] = systems.at(&systemInfo);
        if (--count == 0) //最终移除
        {
            ISystemGroup& systemGroup = systemInfo.group ? *dynamic_cast<ISystemGroup*>(std::get<0>(systems[systemInfo.group])) : rootSystem;
            systemGroup.RemoveSystem(*system);

            //销毁实例
            system->Stop();
            systemInfo.type->Destroy(system);
            systems.erase(&systemInfo);
        }

        if (systemInfo.group != nullptr)
            RemoveSystem(*systemInfo.group);
    }
    void SystemAllocator::Update()
    {
        rootSystem.Update();
    }
}