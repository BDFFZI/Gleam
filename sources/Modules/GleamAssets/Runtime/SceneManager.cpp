#include "SceneManager.h"

#include "GleamPersistence/Runtime/Resources.h"
#include "Scene/SceneAsset.h"

namespace Gleam
{
    void SceneManager::LoadScene(const uuids::uuid assetBundleID)
    {
        AssetBundle& assetBundle = Resources::Load(assetBundleID);
        Scene& scene = SceneAsset::FromAssetBundle(assetBundle);
        scenes.emplace(assetBundle.GetID(), &scene);

        scene.Start();
    }
    void SceneManager::UnloadScene(const uuids::uuid assetBundleID)
    {
        stoppingScenes.push_back(assetBundleID);
    }

    void SceneManager_FlushUnloadingScenes()
    {
        for (auto id : SceneManager::destroyingScenes)
        {
            Scene::Destroy(*SceneManager::scenes[id]);
            Resources::Unload(AssetBundle::GetAssetBundle(id));
            SceneManager::scenes.erase(id);
        }
        SceneManager::destroyingScenes.clear();

        for (auto id : SceneManager::stoppingScenes)
        {
            SceneManager::scenes[id]->Stop();
            SceneManager::destroyingScenes.push_back(id);
        }
        SceneManager::stoppingScenes.clear();
    }
}