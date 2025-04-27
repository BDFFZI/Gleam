#include "Engine.h"

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
    void Engine::Stop()
    {
        isStopping = true;
    }
}