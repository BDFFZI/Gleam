#pragma once
#include "PresentationSystem.h"
#include "LightECS/Runtime/System.hpp"

namespace Light
{
    struct BeginSimulationSystem : System<MinSystemOrder, BeginPresentationSystem>
    {
    };

    struct EndSimulationSystem : System<BeginSimulationSystem, BeginPresentationSystem>
    {
    };

#define SimulationSystemGroup Light::BeginSimulationSystem,Light::EndSimulationSystem
}
