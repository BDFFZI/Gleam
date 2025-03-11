#include "CustomUI.h"

#include "GleamECS/Runtime/Scene.h"
#include "System/ProjectWindow.h"

namespace Gleam
{
    void ProjectWindow_CreateScene()
    {
        auto path = ProjectWindow::GetDirectoryDrawing() / "NewScene.scene";
        Scene& scene = Scene::Create("NewScene");
        AssetBundle& assetBundle = AssetBundle::Create();
        Scene::ToAssetBundle(scene, assetBundle);
        AssetBundle::SaveJson(path.string(), assetBundle);
    }
}