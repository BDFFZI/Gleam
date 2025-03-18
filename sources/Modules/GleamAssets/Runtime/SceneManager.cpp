#include "SceneManager.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "SceneAsset/SceneAsset.h"

namespace Gleam
{
    bool SceneManager::HasScene(const uuids::uuid assetBundleID)
    {
        return allScenes.contains(assetBundleID);
    }
    Scene& SceneManager::LoadScene(const uuids::uuid assetBundleID, const bool isRunning)
    {
        AssetBundle& assetBundle = Resources::Load(assetBundleID);
        Scene& scene = SceneAsset::FromAssetBundle(assetBundle, isRunning);
        allScenes.emplace(assetBundle.GetID(), &scene);

        return scene;
    }
    void SceneManager::UnloadScene(const uuids::uuid assetBundleID)
    {
        Scene::Destroy(*allScenes[assetBundleID]);
        Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
        allScenes.erase(assetBundleID);
    }
    
    void SceneManager_ReleaseScenes()
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