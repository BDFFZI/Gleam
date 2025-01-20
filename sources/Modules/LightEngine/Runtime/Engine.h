#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include "LightECS/Runtime/World.h"


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

#define Light_AddSystems(...) Light_AddInitEvent([](){Light::World::AddSystems({__VA_ARGS__});});
}