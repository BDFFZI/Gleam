#pragma once

#include "SceneManager.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddEngineUpdateEvent(SceneManager_FlushUnloadingScenes, 0)
    Gleam_AddSystems(GlobalSceneManager_RuntimeEvent)

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(GlobalSceneManager_RuntimeEvent)
#endif
}