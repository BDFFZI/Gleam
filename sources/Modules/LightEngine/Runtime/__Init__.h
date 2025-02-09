#pragma once
#include "Engine.h"
#include "LightECS/Runtime/World.h"
#include "System/TimeSystem.h"
#include "System/TransformSystem.h"
#ifdef LightEngineEditor
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

#ifdef LightEngineEditor
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