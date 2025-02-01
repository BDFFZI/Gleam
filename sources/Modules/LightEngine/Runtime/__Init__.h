#pragma once
#include "Engine.h"
#include "System/AwakeSystem.h"
#include "System/DestroySystem.h"
#include "System/SimulationSystem.h"
#include "System/TransformSystem.h"

namespace Light
{
    Light_AddSystems(AwakeSystem)
    Light_AddSystems(DestroySystem)
    Light_AddSystems(SimulationSystem)
    Light_AddSystems(TransformSystem)
}