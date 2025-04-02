#pragma once
#include "CustomUI.h"
#include "EditorSceneManager.h"
#include "GleamAssets/Runtime/__Init__.h"
#include "System/AssetBundleWindow.h"
#include "System/HierarchyWindow_Scene.h"
#include "System/ProjectWindow.h"
#include "GleamEngine/Editor/Editor.h"

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        Engine::AddRuntimeSystems({
            EditorSceneManager_StartScenesSystem
        });
        Editor::AddEditorSystems({
            GlobalProjectWindow,
            GlobalAssetBundleWindow,
            GlobalHierarchyWindow_Scene,
            EditorSceneManager_OpenLastSceneSystem
        });

        InspectorWindow::MakeCustomUI<PersistentEntity>(InspectorWindowUI_EntityAsset);
        ProjectWindow::MakeDirectoryMenu("Create/Scene", ProjectWindowMenu_CreateScene);
        ProjectWindow::MakeFileMenu(".scene", "Open", ProjectWindowMenu_OpenScene);
        ProjectWindow::MakeFileRenameEvent(".scene", ProjectWindowEvent_RenameScene);

        AssetImporter::MakeAssetImporter(".asset", JsonObjectImporterType);
        AssetImporter::MakeAssetImporter(".scene", JsonObjectImporterType);

        Engine::AddStartEvent([]
        {
            if (!std::filesystem::exists("Assets"))
                std::filesystem::create_directory("Assets");
            AssetDatabase::Refresh("Assets");
            if (!std::filesystem::exists("ProjectSettings"))
                std::filesystem::create_directory("ProjectSettings");
            AssetDatabase::Refresh("ProjectSettings");
        }, std::numeric_limits<int>().lowest());
    }
}