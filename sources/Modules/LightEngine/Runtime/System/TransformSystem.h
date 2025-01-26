#pragma once
#include "SimulationSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class TransformSystem : public System
    {
    public:
        TransformSystem(): System(SimulationSystem, OrderRelation::Before)
        {
        }

    private:
        void Update() override;
    };
}