#pragma once
#include "LightECS/Runtime/Scene.h"
#include "LightECS/Runtime/System.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    inline Scene* Awake = World::CreateScene("Awake");
    inline Scene* Destroy = World::CreateScene("Destroy");

    class SimulationSystem : public SystemGroup
    {
    public:
        SimulationSystem(): SystemGroup(std::nullopt, LeftOrder, RightOrder)
        {
        }
    };
    Light_MakeSystem(SimulationSystem)

    class AwakeSystem : public System
    {
    public:
        AwakeSystem(): System(std::nullopt, RightOrder, RightOrder)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystem(AwakeSystem)

    class DestroySystem : public System
    {
    public:
        DestroySystem(): System(std::nullopt, RightOrder, RightOrder)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystem(DestroySystem)
}

// ReSharper disable once CppUnusedIncludeDirective
#include "LightECS/Runtime/World.h"
#include "LightUtility/Runtime/Program.h"
#define Light_AddSystems(...) Light_MakeInitEvent(){::Light::World::AddSystems({__VA_ARGS__});}