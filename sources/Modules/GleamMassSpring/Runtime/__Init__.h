#pragma once
#include "System/CollisionSystem.h"
#include "System/ForceSystem.h"
#include "System/PhysicsSystem.h"
#include "System/PositionSystem.h"
#include "Entity/Archetype.h"

namespace Gleam
{

    Gleam_AddSystems(
        GlobalPhysicsSystem,
        GlobalPositionSystem,
        GlobalForceSystem,
        GlobalCollisionSystem
    )
}