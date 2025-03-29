#include "Engine.h"
#include "GleamECS/Runtime/World/World.h"


namespace Gleam
{
    void Engine::AddStartEvent(const std::function<void()>& event, int order)
    {
        startEvents.insert({order, event});
    }
    void Engine::AddStopEvent(const std::function<void()>& event, int order)
    {
        stopEvents.insert({order, event});
    }
    void Engine::AddUpdateEvent(const std::function<void()>& event, int order)
    {
        updateEvents.insert({order, event});
    }
    void Engine::AddRuntimeSystems(const std::initializer_list<std::reference_wrapper<System>> systems)
    {
        runtimeSystems.insert(runtimeSystems.end(), systems.begin(), systems.end());
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}