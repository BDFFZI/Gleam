#pragma once
#include "Engine.h"
#include "GleamECS/Runtime/World.h"
#include "System/TimeSystem.h"
#include "System/TransformSystem.h"
#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddSystems(
        PreUpdateSystem,
        PostUpdateSystem,
        TimeSystem,
        TransformSystem
    );

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        PreUpdateSystem,
        PostUpdateSystem,
        TransformSystem
    );
#endif
}

inline int main()
{
    Gleam::Engine::Start();
    return 0;
}