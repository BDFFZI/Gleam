#pragma once
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
        static void Start();
        static void Stop();

    private:
        inline static bool isStopping = false;
    };

#include "LightUtility/Runtime/Utility.hpp"
#define Light_AddSystems(...) Light_MakeInitEvent(){Light::World::AddSystems({__VA_ARGS__});}
}