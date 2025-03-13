#pragma once

#include "SceneManager.h"

namespace Gleam
{
    Gleam_AddEngineUpdateEvent(SceneManager_FlushUnloadingScenes, 0)
    Gleam_AddSystems(GlobalSceneManager_RuntimeEvent)
}