#pragma once
#include "ForceSystem.h"
#include "PhysicsSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class PositionSystem : public System
    {
    public:
        PositionSystem(): System(ForceSystem, OrderRelation::After)
        {
        }

        void Update() override;
    };
    Gleam_MakeGlobalSystem(PositionSystem)
}