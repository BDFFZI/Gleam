﻿#include "System.h"

#include <cassert>
#include <iostream>

namespace Light
{
    System::System(const std::optional<SystemGroup*> group, const int minOrder, const int maxOrder)
        : group(group), minOrder(minOrder), maxOrder(maxOrder),
          order(static_cast<int32_t>((static_cast<int64_t>(minOrder) + static_cast<int64_t>(maxOrder)) / 2))
    {
        assert(minOrder <= maxOrder && "最大顺序不能小于最小顺序！");
    }
    System::System(System* system, const OrderRelation orderRelation)
        : System(system->GetGroup(),
                 orderRelation == OrderRelation::Before ? system->minOrder : system->order,
                 orderRelation == OrderRelation::Before ? system->order : system->maxOrder)
    {
    }
    std::optional<SystemGroup*> System::GetGroup() const
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

    SystemEvent::SystemEvent(const std::optional<SystemGroup*> group, const int minOrder, const int maxOrder)
        : System(group, minOrder, maxOrder)
    {
    }
    SystemEvent::SystemEvent(System* system, const OrderRelation orderRelation)
        : System(system, orderRelation)
    {
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

    SystemGroup::SystemGroup(const std::optional<SystemGroup*> group, const int minOrder, const int maxOrder)
        : System(group, minOrder, maxOrder)
    {
    }
    SystemGroup::SystemGroup(System* system, const OrderRelation orderRelation)
        : System(system, orderRelation)
    {
    }
    void SystemGroup::AddSubSystem(System* system)
    {
        subSystemStartQueue.insert(system);
    }
    void SystemGroup::RemoveSubSystem(System* system)
    {
        subSystemUpdateQueue.erase(system);
        subSystemStopQueue.insert(system);
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
            system->Update();
    }
}