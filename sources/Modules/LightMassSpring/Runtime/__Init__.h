#pragma once
#include "LightEngine/Runtime/__Init__.h"

#include "System/ForceSystem.h"
#include "System/MassSpringMeshSystem.h"
#include "System/PhysicsSystem.h"
#include "System/PositionSystem.h"

#ifdef Light_Editor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    Light_AddSystems(
        PhysicsSystem,
        PositionSystem,
        ForceSystem,
        MassSpringMeshSystem
    )

#ifdef Light_Editor
    Light_AddEditorSystems(
        MassSpringMeshSystem
    )
#endif
}