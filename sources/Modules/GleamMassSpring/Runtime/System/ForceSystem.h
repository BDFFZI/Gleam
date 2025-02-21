#pragma once
#include "PhysicsSystem.h"
#include "PositionSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class ForceSystem : public System
    {
    public:
        ForceSystem(): System(PositionSystem, OrderRelation::Before)
        {
        }

        void Update() override;
    };
    Gleam_MakeGlobalSystem(ForceSystem);
}