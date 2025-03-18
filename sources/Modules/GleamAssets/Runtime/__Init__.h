#pragma once

#include "SceneManager.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_AddSystems(SceneManager_ReleaseScenesSystem)

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(SceneManager_ReleaseScenesSystem)
#endif
}