#pragma once
#include "System/MassSpringRenderingSystem.h"
#include "System/PointSelectionSystem.h"

namespace Gleam
{
    Gleam_AddEditorSystems(
        PointSelectionSystem,
        MassSpringRenderingSystem
    )
}