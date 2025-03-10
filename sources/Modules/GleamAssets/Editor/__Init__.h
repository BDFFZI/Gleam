#pragma once
#include "CustomImporter.h"
#include "CustomUI.h"
#include "Asset/AssetImporter.h"
#include "System/ProjectWindow.h"

namespace Gleam
{
    Gleam_AddEditorSystems(
        GlobalProjectWindow
    )

    Gleam_MakeProjectWindowMenu("Create/Scene", ProjectWindow_CreateScene)
    Gleam_MakeImporter(".scene", Type::CreateOrGet<SceneImporter>());
}