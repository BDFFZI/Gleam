#pragma once

#include "System/InspectorWindow.h"
#include "System/HierarchyWindow.h"
#include "System/ProfilerWindow.h"
#include "CustomUI.h"
#include "System/EditorTimeSystem.h"

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        Engine::AddStartEvent(Editor_ReplaceRuntimeSystem, std::numeric_limits<int>::max());
        Engine::AddUpdateEvent(Editor_PlayPauseStopEngine, std::numeric_limits<int>::max());
        Engine::AddUpdateEvent(ProfilerWindow_FetchProfile, std::numeric_limits<int>::max());

        EditorUISystem::MakeEditorMenu("Window/UI/IDStackTool", [] { GlobalEditorUISystem->ShowIDStackToolWindow(); });
        EditorUISystem::MakeEditorMenu("Window/UI/Demo", [] { GlobalEditorUISystem->ShowDemoWindow(); });

        InspectorWindow::MakeCustomUI<Entity>(InspectorWindowUI_Entity);
        InspectorWindow::MakeCustomUI<LocalTransform>(InspectorWindowUI_LocalTransform);
    }
}