#pragma once
#include "PhysicsSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class PositionSystem : public System
    {
    public:
        PositionSystem(): System(PhysicsSystem, MiddleOrder, MiddleOrder)
        {
        }

        void Update() override;
    };
    Gleam_MakeSystemInstance(PositionSystem)
}