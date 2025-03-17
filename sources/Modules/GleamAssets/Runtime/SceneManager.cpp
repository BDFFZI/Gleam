#include "SceneManager.h"

#include "GleamEngine/Editor/Editor.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "SceneAsset/SceneAsset.h"

namespace Gleam
{
    bool SceneManager::HasScene(const uuids::uuid assetBundleID)
    {
        return allScenes.contains(assetBundleID);
    }
    Scene& SceneManager::LoadScene(const uuids::uuid assetBundleID, const bool start)
    {
        AssetBundle& assetBundle = Resources::Load(assetBundleID);
        Scene& scene = SceneAsset::FromAssetBundle(assetBundle);
        allScenes.emplace(assetBundle.GetID(), &scene);

        if (start)
            scene.Start();

        return scene;
    }
    void SceneManager::UnloadScene(const uuids::uuid assetBundleID)
    {
        stoppingScenes.push_back(assetBundleID);
    }
    void SceneManager::UnloadSceneImmediate(const uuids::uuid assetBundleID)
    {
        Scene::Destroy(*allScenes[assetBundleID]);
        Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
        allScenes.erase(assetBundleID);
    }
    void SceneManager::UnloadSceneImmediate(Scene& scene)
    {
        auto it = std::ranges::find_if(allScenes, [&scene](auto& pair)
        {
            return pair.second == &scene;
        });
        UnloadSceneImmediate(it->first);
    }

    void SceneManager_FlushUnloadingScenes()
    {
        for (auto id : SceneManager::destroyingScenes)
        {
            Scene::Destroy(*SceneManager::allScenes[id]);
            Resources::Unload(AssetBundle::GetAssetBundle(id));
            SceneManager::allScenes.erase(id);
        }
        SceneManager::destroyingScenes.clear();

        for (auto id : SceneManager::stoppingScenes)
        {
            if (SceneManager::allScenes[id]->GetIsRunning())
                SceneManager::allScenes[id]->Stop();
            SceneManager::destroyingScenes.push_back(id);
        }
        SceneManager::stoppingScenes.clear();
    }

    void SceneManager_RuntimeEvent::Stop()
    {
        //引擎停止，释放场景（世界负责回收，场景需释放所有权）并回收资源包
        for (auto [id,scene] : SceneManager::allScenes)
        {
            Scene::Destroy(*scene, true);
            Resources::Unload(AssetBundle::GetAssetBundle(id));
        }
        SceneManager::allScenes.clear();
    }
}