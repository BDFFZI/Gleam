#pragma once
#include "LightECS/Runtime/System.h"

namespace Light
{
    class SimulationSystem : public SystemGroup
    {
    public:
        SimulationSystem(): SystemGroup(std::nullopt, LeftOrder, RightOrder)
        {
        }
    };
    Light_MakeSystem(SimulationSystem)
}