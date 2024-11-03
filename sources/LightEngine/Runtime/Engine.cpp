#include "Engine.h"
#include <LightWindow/Runtime/Window.h>

using namespace Light;

void Engine::AddBeginEvent(const std::function<void()>& beginEvent)
{
    engineBeginEvents.push_back(beginEvent);
}
void Engine::AddEndEvent(const std::function<void()>& endEvent)
{
    engineEndEvents.push_back(endEvent);
}
void Engine::AddUpdateEvent(const std::function<void()>& updateEvent)
{
    engineUpdateEvents.push_back(updateEvent);
}

void Engine::Initialize()
{
    Window::Initialize();
}

void Engine::Begin()
{
    Window::SetWindowBeginEvent([]()
    {
        for (const auto& event : engineBeginEvents)
            event();
        engineBeginEvents.clear();
        engineBeginEvents.shrink_to_fit();
    });
    Window::SetWindowEndEvent([]()
    {
        for (const auto& event : engineEndEvents)
            event();
        engineEndEvents.clear();
        engineEndEvents.shrink_to_fit();
    });
    Window::SetWindowUpdateEvent([]()
    {
        for (const auto& event : engineUpdateEvents)
            event();
    });

    Window::Start();
}
void Engine::End()
{
    Window::Stop();
}
