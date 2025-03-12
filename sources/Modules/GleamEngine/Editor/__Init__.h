#pragma once

#include "Editor.h"
#include "CustomEvent.h"
#include "System/InspectorWindow.h"
#include "System/HierarchyWindow.h"
#include "System/ProfilerWindow.h"
#include "CustomUI.h"
#include "System/EditorTimeSystem.h"

namespace Gleam
{
    Gleam_AddEngineEvent(Start, ReplaceRuntimeSystem, 0);
    Gleam_AddEngineEvent(Update, PlayOrStopEngine, 0);
    Gleam_AddEngineEvent(Update, FetchProfile, 1);

    Gleam_AddEditorSystems(
        GlobalEditorUISystem,
        GlobalHierarchyWindow,
        GlobalInspectorWindow,
        GlobalProfilerWindow,
        EditorTimeSystem
    )

    Gleam_AddInspectorWindowUI(Entity, InspectorWindowUI_Entity)
    Gleam_AddInspectorWindowUI(LocalTransform, InspectorWindowUI_LocalTransform)
}