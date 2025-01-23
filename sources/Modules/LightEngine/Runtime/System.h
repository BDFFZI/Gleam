#pragma once
#include "LightECS/Runtime/Scene.h"
#include "LightECS/Runtime/System.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    inline static Scene Awake = World::CreateScene("Awake"); 
    inline static Scene Destroy = World::CreateScene("Destroy"); 
    
    class SimulationSystem : public SystemGroup
    {
    public:
        SimulationSystem(): SystemGroup(nullptr, LeftOrder, RightOrder)
        {
        }
    };
    Light_MakeSystem(SimulationSystem)

    class AwakeSystem : public System
    {
    public:
        AwakeSystem(): System(nullptr, RightOrder, RightOrder)
        {
            
        }
    private:
        void Update() override;
    };
}

#include "LightUtility/Runtime/Program.h"
#include "LightECS/Runtime/World.h"
#define Light_AddSystems(...) Light_MakeInitEvent(){::Light::World::AddSystems({__VA_ARGS__});}