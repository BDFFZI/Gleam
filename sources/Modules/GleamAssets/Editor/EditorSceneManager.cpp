#include "EditorSceneManager.h"

#include "AssetDatabase/AssetDatabase.h"
#include "AssetDatabase/AssetImporter.h"
#include "GleamAssets/Runtime/SceneAssetBundle.h"
#include "GleamAssets/Runtime/SceneManager.h"
#include "GleamUtility/Runtime/Ranges.h"
#include "Configuration/EditorSceneState.h"

namespace Gleam
{
    void EditorSceneManager::OpenScene(const std::filesystem::path& path)
    {
        uuids::uuid assetBundleID = AssetDatabase::GetAssetBundleID(path);
        if (SceneManager::HasScene(assetBundleID))
            SceneManager::UnloadScene(assetBundleID);

        Scene& scene = SceneManager::LoadScene(assetBundleID, false);
        scenePaths[&scene] = path;
        EditorSceneState::lastScenePath = path.string();
    }
    void EditorSceneManager::CloseScene(Scene& scene)
    {
        if (scenePaths.contains(&scene))
        {
            std::filesystem::path scenePath = scenePaths.at(&scene);
            SceneManager::UnloadScene(AssetDatabase::GetAssetBundleID(scenePath));
            if (scenePath == EditorSceneState::lastScenePath)
                EditorSceneState::lastScenePath = "";
        }
        else //非基于序列化的运行时场景
        {
            Scene::Destroy(scene);
        }
    }
    void EditorSceneManager::SaveScene(Scene& scene)
    {
        auto& path = scenePaths.at(&scene);
        AssetBundle& assetBundle = AssetDatabase::GetAssetBundle(path);

        SceneAssetBundle::SaveToAssetBundle(scene, assetBundle);
        AssetDatabase::Save(path);
    }

    void EditorSceneManager_StartScenes()
    {
        for (auto& scene : SceneManager::GetAllScenes() | UnwrapRef)
            scene.Start();
    }
    void EditorSceneManager_OpenLastScene()
    {
        EditorSceneManager::scenePaths.clear();
        if (std::filesystem::exists(EditorSceneState::lastScenePath))
            EditorSceneManager::OpenScene(EditorSceneState::lastScenePath);
    }
}