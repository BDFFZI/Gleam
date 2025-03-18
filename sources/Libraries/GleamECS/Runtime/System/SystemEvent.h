#pragma once
#include "System.h"

namespace Gleam
{
    class SystemEvent : public System
    {
    public:
        static SystemEvent StartEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, int order, std::function<void()> startEvent);
        static SystemEvent UpdateEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, int order, std::function<void()> updateEvent);
        static SystemEvent StopEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, int order, std::function<void()> stopEvent);

        SystemEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, int minOrder = MinOrder, int maxOrder = MaxOrder);
        SystemEvent(const std::string_view& name, System& system, OrderRelation orderRelation);
        SystemEvent(SystemEvent&&) noexcept = default;
        SystemEvent& operator=(SystemEvent&&) = default;

        std::function<void()>& OnStart();
        std::function<void()>& OnStop();
        std::function<void()>& OnUpdate();

    private:
        std::function<void()> onStart = nullptr;
        std::function<void()> onStop = nullptr;
        std::function<void()> onUpdate = nullptr;

        void Start() override;
        void Stop() override;
        void Update() override;
    };

#define Gleam_MakeSystemEvent(action,type,group,order) \
inline SystemEvent action##System = SystemEvent::##type##Event(#action"System",group,order,action);
}