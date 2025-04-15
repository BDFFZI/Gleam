#pragma once
#include "PhysicsSystem.h"

namespace Gleam
{
    class PositionSystem : public System<PhysicsSystem, SystemMinOrder, SystemMidOrder>
    {
        void Update() override;
    };
    Gleam_MakeRuntimeSystem(PositionSystem)
}