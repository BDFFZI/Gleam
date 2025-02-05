#pragma once
#include "PhysicsSystem.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class PositionSystem : public System
    {
    public:
        PositionSystem(): System(PhysicsSystem, MiddleOrder, MiddleOrder)
        {
        }

        void Update() override;
    };
    Light_MakeSystem(PositionSystem)
}