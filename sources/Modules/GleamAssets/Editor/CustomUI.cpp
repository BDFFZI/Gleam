#include "CustomUI.h"

#include "EditorSceneManager.h"
#include "Asset/AssetDatabase.h"
#include "GleamAssets/Runtime/SceneAsset/SceneAsset.h"
#include "System/ProjectWindow.h"

namespace Gleam
{
    void ProjectWindowMenu_CreateScene()
    {
        auto path = ProjectWindow::GetDirectoryDrawing() / "NewScene.scene";

        Scene& scene = Scene::Create("NewScene");
        AssetBundle& assetBundle = AssetBundle::Create();
        SceneAsset::ToAssetBundle(scene, assetBundle);
        AssetDatabase::Save(path, assetBundle);
        AssetBundle::Unload(assetBundle);
        Scene::Destroy(scene);
    }
    void ProjectWindowMenu_OpenScene()
    {
        auto& path = ProjectWindow::GetFileDrawing();
        
        EditorSceneManager::OpenScene(path);
    }
}