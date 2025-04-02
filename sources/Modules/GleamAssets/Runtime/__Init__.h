#pragma once
#include "Configuration.h"
#include "SceneManager.h"
#include "System/ScenePrefabSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        Engine::AddRuntimeSystems({
            SceneManager_ReleaseScenesSystem,
            GlobalScenePrefabSystem
        });
#ifdef GleamEngineEditor
        Editor::AddEditorSystems({
            SceneManager_ReleaseScenesSystem,
            GlobalScenePrefabSystem
        });
#endif
        Engine::AddStartEvent(Configuration_LoadSettings, std::numeric_limits<int>::lowest());
        Engine::AddStopEvent(Configuration_UnloadSettings, std::numeric_limits<int>::max());
    }
}