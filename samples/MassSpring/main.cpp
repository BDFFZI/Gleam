#include "AssetSystem.h"
#include "LogicSystem.h"
#include "GameRenderingSystem.h"

Gleam_AddSystems(
    AssetSystem,
    LogicSystem,
    GameUISystem,
    GameRenderingSystem
)

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
Gleam_AddEditorSystems(
    AssetSystem,
    GameRenderingSystem
)
#endif

Gleam_Main