#include "SystemAllocator.h"

namespace Gleam
{
    IOrderedSystemEvent* SystemAllocator::TryGetSystem(const SystemInfo& systemInfo)
    {
        auto it = systems.find(&systemInfo);
        if (it == systems.end())
            return nullptr;
        auto& [system,count] = it->second;
        return system.get();
    }
    IOrderedSystemEvent& SystemAllocator::AddSystem(const SystemInfo& systemInfo)
    {
        if (systemInfo.group != nullptr)
            AddSystem(*systemInfo.group); //添加引用

        auto& [system,count] = systems[&systemInfo];
        if (++count == 1) //首次添加
        {
            //创建实例
            system = std::static_pointer_cast<IOrderedSystemEvent, void>(systemInfo.type->MakeShared(systemInfo.type->Create()));
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
            //交给Update执行，因为要正确触发Stop事件
        }

        if (systemInfo.group != nullptr)
            RemoveSystem(*systemInfo.group);
    }
    void SystemAllocator::Update()
    {
        rootSystem.Update();
        //Stop事件执行完毕，真正从内存中移除系统
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
        rootSystem.Stop();
        systems.clear();
    }
}