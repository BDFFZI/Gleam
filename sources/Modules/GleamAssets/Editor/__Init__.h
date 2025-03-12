#pragma once
#include "CustomImporter.h"
#include "CustomUI.h"
#include "System/AssetBundleWindow.h"
#include "System/HierarchyWindow_Scene.h"
#include "System/ProjectWindow.h"
#include "GleamEngine/Editor/Editor.h"

namespace Gleam
{
    Gleam_AddEditorSystems(
        GlobalProjectWindow,
        GlobalAssetBundleWindow,
        GlobalHierarchyWindow_Scene
    )

    Gleam_MakeProjectDirectoryMenu("Create/Scene", ProjectWindowMenu_CreateScene)
    Gleam_MakeProjectFileMenu(".scene", "Open", ProjectWindowMenu_OpenScene)

    Gleam_MakeAssetImporter(".asset", Type::CreateOrGet<JsonObjectImporter>());
    Gleam_MakeAssetImporter(".scene", Type::CreateOrGet<JsonObjectImporter>());
}


auto a = Gleam::FieldDataTransferrer_Transfer<Gleam::Entity>::Invoke;