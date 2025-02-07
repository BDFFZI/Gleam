﻿#pragma once
#include "Engine.h"
#include "LightECS/Runtime/World.h"
#include "System/TimeSystem.h"
#include "System/TransformSystem.h"

namespace Light
{
    Light_AddEditorSystems(
        PreUpdateSystem,
        PostUpdateSystem,
        TimeSystem,
        TransformSystem
    );
}

inline int main()
{
    Light::Engine::Start();
    return 0;
}