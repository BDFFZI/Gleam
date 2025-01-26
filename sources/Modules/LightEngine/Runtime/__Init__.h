#pragma once
#include "Engine.h"
#include "System/AwakeSystem.h"
#include "System/DestroySystem.h"
#include "System/SimulationSystem.h"

namespace Light
{
    Light_AddSystems(SimulationSystem)
    Light_AddSystems(AwakeSystem)
    Light_AddSystems(DestroySystem)
}