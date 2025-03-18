#include "SystemEvent.h"

namespace Gleam
{
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
}