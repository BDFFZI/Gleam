#pragma once
#include "SceneManager.h"
#include "SettingManager.h"
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
    }

    Gleam_AddEngineStartEvent(SettingManager_LoadSettings, 0);
    Gleam_AddEngineStopEvent(SettingManager_UnloadSettings, 0);
}