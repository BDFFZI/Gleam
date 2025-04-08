#include "SystemAllocator.h"

namespace Gleam
{
    System& SystemAllocator::AddSystem(SystemInfo& systemInfo)
    {
        if (systemInfo.group != nullptr)
            AddSystem(*systemInfo.group);

        auto& [system,count] = systems[systemInfo.type];
        ++count;

        if (count == 1) //首次添加
        {
            SystemGroup* group = systemInfo.group == nullptr ? &rootSystem : dynamic_cast<SystemGroup*>(std::get<0>(systems[systemInfo.group->type]).get());
            //创建实例
            system = std::shared_ptr<System>(static_cast<System*>(systemInfo.type->Create()));
            system->world = world;
            system->group = group;
            system->order = systemInfo.order;
            //注册到组
            group->AddSubSystem(*system);
        }

        return *system;
    }
    void SystemAllocator::RemoveSystem(SystemInfo& systemInfo)
    {
        if (systemInfo.group != nullptr)
            RemoveSystem(*systemInfo.group);

        auto& [system,count] = systems.at(systemInfo.type);
        --count;

        if (count == 0) //最终移除
        {
            //从组移除
            SystemGroup* group = systemInfo.group == nullptr ? &rootSystem : dynamic_cast<SystemGroup*>(std::get<0>(systems[systemInfo.group->type]).get());
            group->RemoveSubSystem(*system);
            //销毁实例
            //在Update时执行，因为要等待系统Stop事件执行完毕。
        }
    }
    void SystemAllocator::Update()
    {
        rootSystem.Update();

        for (auto it = systems.begin(); it != systems.end();)
        {
            int count = std::get<1>(it->second);
            if (count == 0)
                it = systems.erase(it);
            else
                ++it;
        }
    }
    void SystemAllocator::Clear()
    {
        for (auto& count : systemUsageCount | std::views::values)
            count++; //抑制用户回收方法，防止重复回收
        rootSystem.Stop();
        systemUsageCount.clear();
    }
}