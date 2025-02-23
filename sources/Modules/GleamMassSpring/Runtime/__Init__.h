#pragma once
#include "System/CollisionSystem.h"
#include "System/ForceSystem.h"
#include "System/PhysicsSystem.h"
#include "System/PositionSystem.h"

namespace Gleam
{
    Gleam_AddSystems(
        PhysicsSystem,
        PositionSystem,
        ForceSystem,
        CollisionSystem
    )
}