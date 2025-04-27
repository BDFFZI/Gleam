#pragma once
#include "Configuration.h"
#include "SceneManager.h"
#include "System/ScenePrefabSystem.h"

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        Engine::AddStartEvent(Configuration_LoadSettings, std::numeric_limits<int>::lowest());
        Engine::AddStopEvent(Configuration_UnloadSettings, std::numeric_limits<int>::max());

        Engine::AddUpdateEvent(SceneManager_FlushRemovingScenes, std::numeric_limits<int>::lowest());
        Engine::AddStopEvent(SceneManager_ClearAssetBundle, std::numeric_limits<int>::lowest());
#ifdef GleamEngineEditor
        Editor::AddStopEvent(SceneManager_ClearAssetBundle, std::numeric_limits<int>::lowest());
#endif
    }
}