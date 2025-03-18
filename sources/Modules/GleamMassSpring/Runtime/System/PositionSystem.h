#pragma once
#include "PhysicsSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    class PositionSystem : public System
    {
    public:
        PositionSystem(): System(GlobalPhysicsSystem, MinOrder, DefaultOrder)
        {
        }

        void Update() override;
    };
    Gleam_MakeGlobalSystem(PositionSystem)
}