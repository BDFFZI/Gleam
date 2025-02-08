#include "Engine.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    void Engine::AddStartEvent(const std::function<void()>& event, int order)
    {
        startEvents.insert({order, event});
    }
    void Engine::AddStopEvent(const std::function<void()>& event, int order)
    {
        stopEvents.insert({order, event});
    }
    std::vector<System*>& Engine::RuntimeSystems()
    {
        return runtimeSystems;
    }
    void Engine::Start()
    {
        for (auto* system : runtimeSystems)
            World::AddSystem(system);

        for (auto& event : startEvents | std::views::values)
            event();

        World::Start();
        while (!isStopping)
            World::Update();
        World::Stop();

        for (auto& event : stopEvents | std::views::values)
            event();
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}