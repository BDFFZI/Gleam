#include "EditorSceneManager.h"

#include "Asset/AssetDatabase.h"
#include "Asset/AssetImporter.h"
#include "GleamAssets/Runtime/SceneManager.h"

namespace Gleam
{
    void EditorSceneManager::OpenScene(const std::filesystem::path& path)
    {
        uuids::uuid assetBundleID = AssetDatabase::GetAssetBundleID(path);
        if (SceneManager::HasScene(assetBundleID))
            SceneManager::UnloadSceneImmediate(assetBundleID);

        Scene& scene = SceneManager::LoadScene(assetBundleID, false);
        scenePaths[&scene] = path;
        lastScenePath = path;
    }
    void EditorSceneManager::CloseScene(Scene& scene)
    {
        SceneManager::UnloadSceneImmediate(scene);
    }
    void EditorSceneManager::SaveScene(Scene& scene)
    {
        auto& path = scenePaths.at(&scene);
        AssetBundle& assetBundle = AssetDatabase::GetAssetBundle(path);

        SceneAsset::ToAssetBundle(scene, assetBundle);
        AssetDatabase::Save(path);
    }

    void EditorSceneManager_EditorEvent::Start()
    {
        EditorSceneManager::scenePaths.clear();
        if (!EditorSceneManager::lastScenePath.empty())
            EditorSceneManager::OpenScene(EditorSceneManager::lastScenePath);
    }
}