#include "AssetSystem.h"
#include "LogicSystem.h"

Light_AddSystems(
    AssetSystem,
    LogicSystem,
    GameUISystem
)

#ifdef LightEngineEditor
#include "LightEngine/Editor/Editor.h"
Light_AddEditorSystems(
    AssetSystem
)
#endif