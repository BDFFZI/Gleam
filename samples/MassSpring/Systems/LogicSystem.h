#pragma once
#include "PhysicsSystem.h"
#include "SimulationSystem.h"
#include "LightECS/Runtime/System.hpp"

namespace Light
{
    struct LogicSystem : System<BeginSimulationSystem, PhysicsSystem>
    {
        static void Update();
    };
}
