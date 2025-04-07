#pragma once
#include "Engine.h"
#include "GleamECS/Runtime/World/World.h"
#include "System/TimeSystem.h"
#include "System/TransformSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddRuntimeSystems(
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