#pragma once
#include "CustomImporter.h"
#include "CustomUI.h"
#include "EditorSceneManager.h"
#include "System/AssetBundleWindow.h"
#include "System/HierarchyWindow_Scene.h"
#include "System/ProjectWindow.h"
#include "GleamEngine/Editor/Editor.h"

namespace Gleam
{
    Gleam_AddSystems(
        EditorSceneManager_RuntimeStartEvent
    )

    Gleam_AddEditorSystems(
        GlobalProjectWindow,
        GlobalAssetBundleWindow,
        GlobalHierarchyWindow_Scene,
        EditorSceneManager_EditorStartEvent
    )

    Gleam_AddProjectWindowDirectoryMenu("Create/Scene", ProjectWindowMenu_CreateScene)
    Gleam_AddProjectWindowFileMenu(".scene", "Open", ProjectWindowMenu_OpenScene)

    Gleam_MakeAssetImporter(".asset", Type::CreateOrGet<JsonObjectImporter>());
    Gleam_MakeAssetImporter(".scene", Type::CreateOrGet<JsonObjectImporter>());
}