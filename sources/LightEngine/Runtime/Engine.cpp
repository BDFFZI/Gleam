#include "Engine.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    void Engine::AddInitEvent(const std::function<void()>& initEvent)
    {
        initEvents.push_back(initEvent);
    }
    void Engine::Start()
    {
        //外部自定义初始化
        for (const auto& event : initEvents)
            event();
        initEvents = {};

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