#pragma once
#include "Engine.h"
#include "GleamECS/Runtime/World/World.h"
#include "System/TimeSystem.h"
#include "System/TransformSystem.h"
#include "Entity/Archetype.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddSystems(
        GlobalPreUpdateSystem,
        GlobalPostUpdateSystem,
        GlobalTimeSystem,
        GlobalTransformSystem
    );

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        GlobalPreUpdateSystem,
        GlobalPostUpdateSystem,
        GlobalTransformSystem
    );
#endif
}