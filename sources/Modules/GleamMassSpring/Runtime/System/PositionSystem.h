#pragma once
#include "PhysicsSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class PositionSystem : public System
    {
    public:
        PositionSystem(): System(PhysicsSystem, MinOrder, DefaultOrder)
        {
        }

        void Update() override;
    };
    Gleam_MakeGlobalSystem(PositionSystem)
}