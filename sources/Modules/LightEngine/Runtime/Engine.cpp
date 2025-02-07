#include "Engine.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    std::vector<System*>& Engine::GetRuntimeSystems()
    {
        return runtimeSystems;
    }
    std::vector<System*>& Engine::GetEditorSystems()
    {
        return editorSystems;
    }
    void Engine::Start()
    {
        for (auto* system : editorSystems)
            World::AddSystem(system);

        World::Start();
        while (!isStopping)
            World::Update();
        World::Stop();
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}