#pragma once
#include "LightECS/Runtime/System.h"

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
}

#include "LightUtility/Runtime/Program.h"
#include "LightECS/Runtime/World.h"
#define Light_AddSystems(...) Light_MakeInitEvent(){Light::World::AddSystems({__VA_ARGS__});}