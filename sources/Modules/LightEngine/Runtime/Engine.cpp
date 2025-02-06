#include "Engine.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    void Engine::Start()
    {
        World::Start();
        while (!isStopping)
            World::Update(!isUpdating);
        World::Stop();
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}