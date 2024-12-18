#include "Engine.h"
#include <LightWindow/Runtime/Window.h>

#include "LightECS/Runtime/World.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    void Engine::AddLaunchEvent(const std::function<void()>& launchEvent)
    {
        launchEvents.push_back(launchEvent);
    }

    void Engine::Launch()
    {
        //核心模块初始化
        Window window = Window::Initialize("LightEngine", static_cast<int>(1920 * 0.7f), static_cast<int>(1080 * 0.7f), false);
        std::vector<const char*> extensions;
        Graphics::InitializeGLDemand(extensions);
        GL gl = GL::Initialize(Window::GetGlfwWindow(), extensions);
        Graphics graphics = Graphics::Initialize(gl);
        UI::Initialize(window, graphics);

        //外部自定义初始化
        for (const auto& event : launchEvents)
            event();
        launchEvents = {};

        //开始逻辑更新
        Window::SetWindowStartEvent([]
        {
            World::Start();
        });
        Window::SetWindowUpdateEvent([]
        {
            World::Update();
        });
        Window::SetWindowStopEvent([]
        {
            World::Stop();
        });
        Window::Start();

        //核心模块逆初始化
        UI::UnInitialize();
        Graphics::UnInitialize();
        GL::UnInitialize();
    }
}