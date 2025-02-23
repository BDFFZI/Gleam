#include "Engine.h"
#include <ranges>

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
    void Engine::Start()
    {
        for (auto system : runtimeSystems)
            World::AddSystem(system);

        for (auto& event : startEvents | std::views::values)
            event();

        while (!isStopping)
        {
            World::Update();
            for (auto& event : updateEvents | std::views::values)
                event();
        }
        World::Clear();

        for (auto& event : stopEvents | std::views::values)
            event();
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}