#pragma once
#include "Engine.h"
#include "LightECS/Runtime/World.h"
#include "System/TimeSystem.h"
#include "System/TransformSystem.h"
#ifdef Light_Editor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    Light_AddSystems(
        PreUpdateSystem,
        PostUpdateSystem,
        TimeSystem,
        TransformSystem
    );

#ifdef Light_Editor
    Light_AddEditorSystems(
        PreUpdateSystem,
        PostUpdateSystem,
        TransformSystem
    );
#endif
}

inline int main()
{
    Light::Engine::Start();
    return 0;
}