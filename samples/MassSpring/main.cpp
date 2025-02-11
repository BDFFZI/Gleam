#include "AssetSystem.h"
#include "LogicSystem.h"

Gleam_AddSystems(
    AssetSystem,
    LogicSystem,
    GameUISystem
)

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
Gleam_AddEditorSystems(
    AssetSystem
)
#endif

Gleam_Main