#pragma once
#include <iostream>

#include "SceneManager.h"
#include "Scene/EntityTransferrer.h"

namespace Gleam
{
    Gleam_AddEngineUpdateEvent(SceneManager_FlushUnloadingScenes, 0)
}