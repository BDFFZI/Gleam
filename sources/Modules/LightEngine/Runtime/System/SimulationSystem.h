#pragma once
#include "LightECS/Runtime/System.h"

namespace Light
{
    class SimulationSystem : public SystemGroup
    {
    public:
        SimulationSystem(): SystemGroup(std::nullopt, MiddleOrder, RightOrder)
        {
        }
    };
    Light_MakeSystem(SimulationSystem)
}