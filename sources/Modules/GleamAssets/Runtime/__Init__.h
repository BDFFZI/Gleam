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
    }
}