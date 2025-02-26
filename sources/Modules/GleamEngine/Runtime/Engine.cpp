#include "Engine.h"
#include "GleamECS/Runtime/World.h"


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
    std::vector<std::reference_wrapper<System>>& Engine::RuntimeSystems()
    {
        return runtimeSystems;
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}