#pragma once

#include "System/ForceSystem.h"
#include "System/MassSpringMeshSystem.h"
#include "System/PhysicsSystem.h"
#include "System/PositionSystem.h"

namespace Light
{
    Light_AddSystems(
        PhysicsSystem,
        PositionSystem,
        ForceSystem,
        MassSpringMeshSystem
    )
}

#include "LightEngine/Runtime/__Init__.h"