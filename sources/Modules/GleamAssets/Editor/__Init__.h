#pragma once
#include "CustomUI.h"
#include "EditorSceneManager.h"
#include "System/AssetBundleWindow.h"
#include "System/HierarchyWindow_Scene.h"
#include "System/ProjectWindow.h"
#include "GleamEngine/Editor/Editor.h"

namespace Gleam
{
    Gleam_MakeEngineStartEvent(RefreshAssets, SettingManager_LoadSettingsOrder-1)
    {
        if (!std::filesystem::exists("Assets"))
            std::filesystem::create_directory("Assets");
        AssetDatabase::Refresh("Assets");
        if (!std::filesystem::exists("ProjectSettings"))
            std::filesystem::create_directory("ProjectSettings");
        AssetDatabase::Refresh("ProjectSettings");
    }

    Gleam_AddSystems(
        EditorSceneManager_StartScenesSystem
    )

    Gleam_AddEditorSystems(
        GlobalProjectWindow,
        GlobalAssetBundleWindow,
        GlobalHierarchyWindow_Scene,
        EditorSceneManager_OpenLastSceneSystem
    )

    Gleam_AddInspectorWindowUI(EntityAsset, InspectorWindowUI_EntityAsset)

    Gleam_AddProjectWindowDirectoryMenu("Create/Scene", ProjectWindowMenu_CreateScene)
    Gleam_AddProjectWindowFileMenu(".scene", "Open", ProjectWindowMenu_OpenScene)

    Gleam_MakeAssetImporter(".asset", Type::CreateOrGet<JsonObjectImporter>());
    Gleam_MakeAssetImporter(".scene", Type::CreateOrGet<JsonObjectImporter>());
}