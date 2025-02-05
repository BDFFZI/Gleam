#pragma once
#include "System/AwakeSystem.h"
#include "System/DestroySystem.h"
#include "System/TimeSystem.h"
#include "System/TransformSystem.h"

namespace Light
{
    Light_AddSystems(AwakeSystem)
    Light_AddSystems(DestroySystem)
    Light_AddSystems(PreUpdateSystem)
    Light_AddSystems(PostUpdateSystem)
    Light_AddSystems(TimeSystem)
    Light_AddSystems(TransformSystem)
}

inline int main()
{
    Light::Engine::Start();
    return 0;
}