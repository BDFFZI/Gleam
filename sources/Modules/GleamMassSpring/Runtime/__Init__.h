#pragma once
#include "System/ForceSystem.h"
#include "System/PhysicsSystem.h"
#include "System/PositionSystem.h"

#ifdef GleamRenderingRuntime
#include "System/MassSpringMeshSystem.h"
#endif

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddSystems(
        PhysicsSystem,
        PositionSystem,
        ForceSystem,
    )

#ifdef GleamRenderingRuntime
    Gleam_AddSystems(MassSpringMeshSystem)
#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        MassSpringMeshSystem
    )
#endif
#endif
}