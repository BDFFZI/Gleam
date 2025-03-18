#include "System.h"
#include <ranges>

namespace Gleam
{
    std::optional<std::reference_wrapper<System>> System::GetGlobalSystem(const uuids::uuid id)
    {
        if (allGlobalSystems.contains(id))
            return *allGlobalSystems.at(id);
        return std::nullopt;
    }

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
    const std::string& System::GetName() const
    {
        return name;
    }
    uuids::uuid System::GetID() const
    {
        return id;
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


    SystemEvent SystemEvent::StartEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, const int order, std::function<void()> startEvent)
    {
        SystemEvent systemEvent = SystemEvent(name, group, order, order);
        systemEvent.onStart = std::move(startEvent);
        return systemEvent;
    }
    SystemEvent SystemEvent::UpdateEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, const int order, std::function<void()> updateEvent)
    {
        SystemEvent systemEvent = SystemEvent(name, group, order, order);
        systemEvent.onUpdate = std::move(updateEvent);
        return systemEvent;
    }
    SystemEvent SystemEvent::StopEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, const int order, std::function<void()> stopEvent)
    {
        SystemEvent systemEvent = SystemEvent(name, group, order, order);
        systemEvent.onStop = std::move(stopEvent);
        return systemEvent;
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

            for (auto* subSystem : systemGroup->addingSystems)
            {
                outSubSystems.emplace_back(*subSystem);
                if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                    subSystemGroups.push_back(subSystemGroup);
            }

            for (auto* subSystem : systemGroup->subSystems)
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
        assert(!addingSystems.contains(&system) && "重复添加系统！");

        addingSystems.insert(&system);
    }
    void SystemGroup::RemoveSubSystem(System& system)
    {
        assert(!removingSystems.contains(&system) && "重复移除系统！");

        removingSystems.insert(&system);
    }

    void SystemGroup::FlushStartQueue()
    {
        for (System* system : addingSystems)
            subSystems.emplace(system);
        for (System* system : subSystems)
        {
            if (addingSystems.contains(system))
                system->Start();
            if (SystemGroup* systemGroup = dynamic_cast<SystemGroup*>(system))
                systemGroup->FlushStartQueue();
        }
        addingSystems.clear();
    }
    void SystemGroup::FlushStopQueue()
    {
        for (System* system : std::ranges::reverse_view(subSystems))
        {
            if (SystemGroup* systemGroup = dynamic_cast<SystemGroup*>(system))
                systemGroup->FlushStopQueue();
            if (removingSystems.contains(system))
                system->Stop();
        }
        for (System* system : removingSystems)
            subSystems.erase(system);
        removingSystems.clear();
    }
    void SystemGroup::Clear()
    {
        for (System* system : std::ranges::reverse_view(subSystems))
        {
            if (SystemGroup* systemGroup = dynamic_cast<SystemGroup*>(system))
                systemGroup->Clear();
            system->Stop();
        }

        addingSystems.clear();
        removingSystems.clear();
        subSystems.clear();
    }
}