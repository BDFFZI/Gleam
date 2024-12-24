#include "Engine.h"

namespace Light
{
    void Engine::AddInitEvent(const std::function<void()>& initEvent)
    {
        initEvents.push_back(initEvent);
    }
    void Engine::AddUnInitEvent(const std::function<void()>& unInitEvent)
    {
        unInitEvents.push_back(unInitEvent);
    }
    void Engine::Start()
    {
        for (const auto& event : initEvents)
            event();

        World::Start();
        while (isStopping == false)
        {
            World::Update();
        }
        World::Stop();

        for (const auto& event : unInitEvents)
            event();
    }
    void Engine::Stop()
    {
        isStopping = true;
    }
}