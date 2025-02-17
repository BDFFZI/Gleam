#include "System.h"

#include <iostream>
#include <ranges>

namespace Gleam
{
    System::System()
        : System(std::nullopt)
    {
    }
    System::System(
        const std::optional<std::reference_wrapper<SystemGroup>> group,
        const int minOrder, const int maxOrder, const std::string_view name)
        : name(name), group(group), minOrder(minOrder), maxOrder(maxOrder),
          order(static_cast<int32_t>((static_cast<int64_t>(minOrder) + static_cast<int64_t>(maxOrder)) / 2))
    {
    }
    System::System(System& system, const OrderRelation orderRelation, const std::string_view name)
        : System(system.GetGroup(),
                 orderRelation == OrderRelation::Before ? system.minOrder : system.order,
                 orderRelation == OrderRelation::Before ? system.order : system.maxOrder, name)
    {
    }
    System::System(SystemGroup& group)
        : System(std::optional<std::reference_wrapper<SystemGroup>>(group))
    {
    }
    std::string& System::GetName()
    {
        return name;
    }
    std::optional<std::reference_wrapper<SystemGroup>> System::GetGroup() const
    {
        return group;
    }
    int System::GetOrder() const
    {
        return order;
    }
    void System::Start()
    {
    }
    void System::Stop()
    {
    }
    void System::Update()
    {
    }


    SystemEvent::SystemEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, const int minOrder, const int maxOrder)
        : System(group, minOrder, maxOrder, name)
    {
    }
    SystemEvent::SystemEvent(const std::string_view& name, System& system, const OrderRelation orderRelation): System(system, orderRelation, name)
    {
    }
    std::function<void()>& SystemEvent::OnStart()
    {
        return onStart;
    }
    std::function<void()>& SystemEvent::OnStop()
    {
        return onStop;
    }
    std::function<void()>& SystemEvent::OnUpdate()
    {
        return onUpdate;
    }
    void SystemEvent::Start()
    {
        if (onStart) onStart();
    }
    void SystemEvent::Stop()
    {
        if (onStop) onStop();
    }
    void SystemEvent::Update()
    {
        if (onUpdate) onUpdate();
    }

    SystemGroup::SystemGroup(const std::optional<std::reference_wrapper<SystemGroup>>& group, const int minOrder, const int maxOrder, const std::string_view& name)
        : System(group, minOrder, maxOrder, name)
    {
    }
    SystemGroup::SystemGroup(System& system, const OrderRelation orderRelation, const std::string_view& name)
        : System(system, orderRelation, name)
    {
    }
    SystemGroup::SystemGroup(SystemGroup& group): System(group)
    {
    }
    std::vector<std::reference_wrapper<System>> SystemGroup::GetSubSystems()
    {
        std::vector<std::reference_wrapper<System>> outSubSystems;

        std::vector subSystemGroups = {this};
        while (!subSystemGroups.empty())
        {
            SystemGroup* systemGroup = subSystemGroups.back();
            subSystemGroups.pop_back();

            for (auto* subSystem : systemGroup->subSystemStartQueue)
            {
                outSubSystems.emplace_back(*subSystem);
                if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                    subSystemGroups.push_back(subSystemGroup);
            }

            for (auto* subSystem : systemGroup->subSystemUpdateQueue)
            {
                outSubSystems.emplace_back(*subSystem);
                if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                    subSystemGroups.push_back(subSystemGroup);
            }
        }
        return outSubSystems;
    }
    void SystemGroup::AddSubSystem(System& system)
    {
        subSystemStartQueue.insert(&system);
    }
    void SystemGroup::RemoveSubSystem(System& system)
    {
        System* systemPtr = &system;
        if (subSystemStartQueue.contains(systemPtr))
            subSystemStartQueue.erase(systemPtr);
        else if (subSystemUpdateQueue.contains(systemPtr))
        {
            subSystemUpdateQueue.erase(systemPtr);
            subSystemStopQueue.insert(systemPtr);
        }
        else if (subSystemStopQueue.contains(systemPtr))
            throw std::runtime_error("不能重复移除系统！");
        else
            throw std::runtime_error("不能移除尚未添加过的系统！");
    }
    void SystemGroup::Start()
    {
        if (subSystemStartQueue.empty() == false)
        {
            for (System* system : subSystemStartQueue)
                system->Start();
            subSystemUpdateQueue.insert(subSystemStartQueue.begin(), subSystemStartQueue.end());
            subSystemStartQueue.clear();
        }
    }
    void SystemGroup::Stop()
    {
        if (subSystemStopQueue.empty() == false)
        {
            for (System* system : std::ranges::reverse_view(subSystemStopQueue))
                system->Stop();
            subSystemStopQueue.clear();
        }

        if (subSystemUpdateQueue.empty() == false)
        {
            for (System* system : std::ranges::reverse_view(subSystemUpdateQueue))
                system->Stop();
            subSystemUpdateQueue.clear();
        }

        subSystemStartQueue.clear();
    }
    void SystemGroup::Update()
    {
        if (subSystemStopQueue.empty() == false)
        {
            for (System* system : std::ranges::reverse_view(subSystemStopQueue))
                system->Stop();
            subSystemStopQueue.clear();
        }

        if (subSystemStartQueue.empty() == false)
        {
            for (System* system : subSystemStartQueue)
                system->Start();
            subSystemUpdateQueue.insert(subSystemStartQueue.begin(), subSystemStartQueue.end());
            subSystemStartQueue.clear();
        }

        for (System* system : subSystemUpdateQueue)
        {
            system->Update();
        }
    }
}