#include "Engine.h"

namespace Light
{
    void Engine::Start()
    {
        World::Start();
        while (isStopping == false)
        {
            World::Update();
        }
        World::Stop();
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}