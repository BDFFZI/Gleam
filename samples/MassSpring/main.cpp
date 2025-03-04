#include "AssetSystem.h"
#include "LogicSystem.h"
#include "GameRenderingSystem.h"

Gleam_AddSystems(
    GlobalAssetSystem,
    GlobalLogicSystem,
    GlobalGameUISystem,
    GlobalGameRenderingSystem
)

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
Gleam_AddEditorSystems(
    GlobalAssetSystem,
    GlobalGameRenderingSystem
)
#endif

Gleam_Main