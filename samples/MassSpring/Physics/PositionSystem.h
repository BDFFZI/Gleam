#pragma once
#include "PhysicsSystem.h"
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class PositionSystem : public System
    {
    public:
        PositionSystem(): System(&PhysicsSystem, MiddleOrder)
        {
        }

        void Update() override;
    };
    inline PositionSystem PositionSystem = {};
}
