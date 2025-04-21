#pragma once
#include "CustomUI.h"
#include "EditorSceneManager.h"
#include "GleamAssets/Runtime/__Init__.h"
#include "GleamEngine/Editor/Editor.h"

// ReSharper disable CppUnusedIncludeDirective
#include "System/ProjectWindow.h"
#include "System/HierarchyWindow_Scene.h"
#include "System/AssetBundleWindow.h"
// ReSharper restore CppUnusedIncludeDirective

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        Editor::AddPlayEvent(EditorSceneManager_RuntimeStart);
        Editor::AddStopEvent(EditorSceneManager_RuntimeStop);

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