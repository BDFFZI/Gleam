#pragma once
#include "SimulationSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class TransformSystem : public System
    {
    public:
        TransformSystem(): System(SimulationSystem)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystem(TransformSystem)
}