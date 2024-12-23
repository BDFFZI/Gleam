#pragma once
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class SimulationSystem : public SystemGroup
    {
    public:
        SimulationSystem(): SystemGroup(nullptr, LeftOrder, MiddleOrder)
        {
        }
    };
    inline SimulationSystem SimulationSystem = {};
}
