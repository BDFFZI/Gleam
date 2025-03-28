#include "CustomUI.h"
#include "GleamEngine/Editor/CustomUI.h"

#include "EditorSceneManager.h"
#include "Asset/AssetDatabase.h"
#include "GleamAssets/Runtime/Asset/SceneAsset.h"
#include "System/ProjectWindow.h"

namespace Gleam
{
    void ProjectWindowMenu_CreateScene()
    {
        auto path = ProjectWindow::GetDirectoryDrawing() / "NewScene.scene";

        Scene& scene = Scene::Create("NewScene");
        AssetBundle& assetBundle = AssetBundle::Create();
        SceneAsset::ToAssetBundle(scene, assetBundle);
        AssetDatabase::Create(path, assetBundle);
        AssetBundle::Unload(assetBundle);
        Scene::Destroy(scene);
    }
    void ProjectWindowMenu_OpenScene()
    {
        auto& path = ProjectWindow::GetFileDrawing();

        EditorSceneManager::OpenScene(path);
    }
    void ProjectWindowMenu_RenameScene()
    {
        auto& path = ProjectWindow::GetFileDrawing();

        EditorSceneManager::OpenScene(path);
    }

    void InspectorWindowUI_EntityAsset(EntityAsset& entityAsset)
    {
        InspectorWindowUI_Entity(entityAsset.GetEntity());
    }
}