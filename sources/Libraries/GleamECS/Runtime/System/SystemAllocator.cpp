#include "SystemAllocator.h"

namespace Gleam
{
    IOrderedSystemEvent& SystemAllocator::AddSystem(const SystemInfo& systemInfo)
    {
        if (systemInfo.group != nullptr)
            AddSystem(*systemInfo.group); //添加引用

        auto& [system,count] = systems[&systemInfo];
        if (++count == 1) //首次添加
        {
            //创建实例
            system = std::static_pointer_cast<IOrderedSystemEvent, void>(systemInfo.type->MakeShared(systemInfo.type->Create()));
            system->Start();
            //添加到组
            ISystemGroup& systemGroup = systemInfo.group ? *dynamic_cast<ISystemGroup*>(std::get<0>(systems[systemInfo.group]).get()) : rootSystem;
            systemGroup.AddSystem(*system);
        }
        return *system;
    }
    void SystemAllocator::RemoveSystem(const SystemInfo& systemInfo)
    {
        auto& [system,count] = systems.at(&systemInfo);
        if (--count == 0) //最终移除
        {
            //移除出组
            ISystemGroup& systemGroup = systemInfo.group ? *dynamic_cast<ISystemGroup*>(std::get<0>(systems[systemInfo.group]).get()) : rootSystem;
            systemGroup.RemoveSystem(*system);
            //销毁实例
            system->Stop();
            systems.erase(&systemInfo);
        }

        if (systemInfo.group != nullptr)
            RemoveSystem(*systemInfo.group);
    }
    void SystemAllocator::Update()
    {
        rootSystem.Update();
    }
    void SystemAllocator::Clear()
    {
        std::vector<std::tuple<const SystemInfo*, int>> usageCount;
        usageCount.reserve(systems.size());
        for (auto& system : systems)
            usageCount.emplace_back(system.first, std::get<1>(system.second));

        std::ranges::sort(usageCount, [](std::tuple<const SystemInfo*, int>& left, std::tuple<const SystemInfo*, int>& right)
        {
            if (std::get<0>(left)->isGroup ^ std::get<0>(right)->isGroup)
                return std::get<0>(left)->isGroup < std::get<0>(right)->isGroup;
            return std::get<1>(left) < std::get<1>(right);
        });

        for (auto& [system,count] : usageCount)
        {
            if (systems.contains(system))
                RemoveSystem(*system);
        }
    }
}