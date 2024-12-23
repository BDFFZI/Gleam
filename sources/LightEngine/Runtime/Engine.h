#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "ECS/World.h"

namespace Light
{
    class Engine
    {
    public:
        static void AddInitEvent(const std::function<void()>& initEvent);
        static void Start();
        static void Stop();

    private:
        inline static std::vector<std::function<void()>> initEvents;
        inline static bool isStopping = false;
    };

#define Light_AddInitEvent_Inner2(initEvent,id)\
int initEventRegister##id = []{Light::Engine::AddInitEvent(initEvent);return 0;}();
#define Light_AddInitEvent_Inner(initEvent,id) Light_AddInitEvent_Inner2(initEvent,id)
#define Light_AddInitEvent(initEvent) Light_AddInitEvent_Inner(initEvent,__COUNTER__)


#define Light_AddSystems(...) Light_AddInitEvent([](){Light::World::AddSystems({__VA_ARGS__});});
}