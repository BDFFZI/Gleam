#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "ECS/World.h"


namespace Light
{
    class SimulationSystem : public SystemGroup
    {
    public:
        SimulationSystem(): SystemGroup(nullptr, LeftOrder, RightOrder)
        {
        }
    };
    Light_MakeSystem(SimulationSystem)

    class Engine
    {
    public:
        static void AddInitEvent(const std::function<void()>& initEvent);
        static void AddUnInitEvent(const std::function<void()>& unInitEvent);
        static void Start();
        static void Stop();

    private:
        inline static std::vector<std::function<void()>> initEvents;
        inline static std::vector<std::function<void()>> unInitEvents;
        inline static bool isStopping = false;
    };

    class ModuleRegister
    {
    public:
        ModuleRegister(const std::initializer_list<System*> systems, const std::function<void()>& initEvent = nullptr, const std::function<void()>& unInitEvent = nullptr)
        {
            World::AddSystems(systems);
            if (initEvent != nullptr)
                Engine::AddInitEvent(initEvent);
            if (unInitEvent != nullptr)
                Engine::AddUnInitEvent(unInitEvent);
        }
    };

#define Light_AddInitEvent_Inner2(initEvent,id)\
inline int initEventRegister##id = []{Light::Engine::AddInitEvent(initEvent);return 0;}();
#define Light_AddInitEvent_Inner(initEvent,id)\
Light_AddInitEvent_Inner2(initEvent,id)

#define Light_AddInitEvent(initEvent) Light_AddInitEvent_Inner(initEvent,__COUNTER__)
#define Light_AddSystems(...) Light_AddInitEvent([](){Light::World::AddSystems({__VA_ARGS__});});
}