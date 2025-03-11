#include "CustomUI.h"

#include "EditorSceneManager.h"
#include "Asset/AssetDatabase.h"
#include "GleamECS/Runtime/Scene.h"
#include "System/ProjectWindow.h"

namespace Gleam
{
    void ProjectWindowMenu_CreateScene()
    {
        auto path = ProjectWindow::GetDirectoryDrawing() / "NewScene.scene";
        Scene& scene = Scene::Create("NewScene");

        AssetBundle& assetBundle = AssetBundle::Create();
        {
            Scene::ToAssetBundle(scene, assetBundle);
            AssetDatabase::Save(path, assetBundle);
        }
        AssetBundle::UnLoad(assetBundle);
    }
    void ProjectWindowMenu_OpenScene()
    {
        auto& path = ProjectWindow::GetFileDrawing();
        EditorSceneManager::OpenScene(path);
    }
}