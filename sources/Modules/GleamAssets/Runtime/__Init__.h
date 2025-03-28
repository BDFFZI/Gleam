#pragma once
#include "SceneManager.h"
#include "SettingManager.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddSystems(SceneManager_ReleaseScenesSystem)

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(SceneManager_ReleaseScenesSystem)
#endif

    Gleam_AddEngineStartEvent(SettingManager_LoadSettings, 0);
    Gleam_AddEngineStopEvent(SettingManager_UnloadSettings, 0);
}