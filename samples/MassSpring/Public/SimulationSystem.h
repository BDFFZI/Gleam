#pragma once
#include "LightECS/Runtime/System.h"

namespace Light
{
    class SimulationSystem : public SystemGroup
    {
    public:
        SimulationSystem(): SystemGroup(nullptr, LeftOrder, MiddleOrder)
        {
        }
    };
    Light_MakeSystem(SimulationSystem)
}