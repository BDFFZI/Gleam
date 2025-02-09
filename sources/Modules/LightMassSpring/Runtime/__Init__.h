#pragma once
#include "LightEngine/Runtime/__Init__.h"

#include "System/ForceSystem.h"
#include "System/PhysicsSystem.h"
#include "System/PositionSystem.h"

#ifdef LightRenderingRuntime
#include "System/MassSpringMeshSystem.h"
#endif

#ifdef LightEngineEditor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    Light_AddSystems(
        PhysicsSystem,
        PositionSystem,
        ForceSystem,
    )

#ifdef LightRenderingRuntime
    Light_AddSystems(MassSpringMeshSystem)
#ifdef LightEngineEditor
    Light_AddEditorSystems(
        MassSpringMeshSystem
    )
#endif
#endif
}