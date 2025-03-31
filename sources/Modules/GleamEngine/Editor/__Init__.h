#pragma once

#include "Editor.h"
#include "System/InspectorWindow.h"
#include "System/HierarchyWindow.h"
#include "System/ProfilerWindow.h"
#include "CustomUI.h"
#include "System/EditorTimeSystem.h"

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        Engine::AddStartEvent(Editor_InterceptRuntimeSystem, std::numeric_limits<int>::max());
        Engine::AddUpdateEvent(Editor_PlayOrStopEngine, std::numeric_limits<int>::max());
        Engine::AddUpdateEvent(ProfilerWindow_FetchProfile, std::numeric_limits<int>::max());

        Editor::AddEditorSystems({
            GlobalEditorUISystem,
            GlobalHierarchyWindow,
            GlobalInspectorWindow,
            GlobalProfilerWindow,
            EditorTimeSystem
        });

        EditorUISystem::MakeEditorMenu("Window/UI/IDStackTool", EditorUISystem::ShowIDStackToolWindow);
        EditorUISystem::MakeEditorMenu("Window/UI/Demo", EditorUISystem::ShowDemoWindow);

        InspectorWindow::MakeCustomUI<Entity>(InspectorWindowUI_Entity);
        InspectorWindow::MakeCustomUI<LocalTransform>(InspectorWindowUI_LocalTransform);
    }
}