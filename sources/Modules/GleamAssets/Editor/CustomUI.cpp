#include "CustomUI.h"
#include "GleamEngine/Editor/CustomUI.h"

#include "EditorSceneManager.h"
#include "AssetDatabase/AssetDatabase.h"
#include "GleamAssets/Runtime/SceneAssetBundle.h"
#include "GleamAssets/Runtime/Asset/BasicSceneInfo.h"
#include "System/ProjectWindow.h"

namespace Gleam
{
    void ProjectWindowMenu_CreateScene()
    {
        auto path = ProjectWindow::GetDirectoryDrawing() / "NewScene.scene";

        Scene& scene = World::AddScene("NewScene");
        AssetBundle& assetBundle = AssetBundle::Create();
        SceneAssetBundle::SaveToAssetBundle(scene, assetBundle);
        AssetDatabase::Create(path, assetBundle);
        AssetBundle::Unload(assetBundle);
        World::RemoveScene(scene);
    }
    void ProjectWindowMenu_OpenScene()
    {
        auto& path = ProjectWindow::GetFileDrawing();

        EditorSceneManager::OpenScene(path);
    }
    void ProjectWindowEvent_RenameScene(const std::filesystem::path& oldPath, const std::filesystem::path& newPath)
    {
        AssetBundle& assetBundle = AssetDatabase::Load(oldPath);
        assetBundle.GetObject<BasicSceneInfo>(0).name = newPath.stem().string();
        AssetDatabase::Save(oldPath);
        AssetDatabase::Unload(oldPath);
    }

    void InspectorWindowUI_EntityAsset(PersistentEntity& entityAsset)
    {
        InspectorWindowUI_Entity(entityAsset.GetEntity());
    }
}