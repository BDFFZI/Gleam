#include "SceneManager.h"

#include "SceneAssetBundle.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "Configuration/SceneSetting.h"

namespace Gleam
{
    bool SceneManager::HasScene(const uuids::uuid assetBundleID)
    {
        return allScenes.contains(assetBundleID);
    }
    Scene& SceneManager::LoadScene(const uuids::uuid assetBundleID, const bool isRunning)
    {
        //卸载已有场景（目前不能先加载再卸载，虽然这理论可以减少回收工作，但由于资源包的所有权已被场景剥夺，绕过了引用计数判断，因此先加载后，相关资源最终还是会被卸载）
        std::vector<uuids::uuid> oldScenes = {};
        std::ranges::copy(allScenes | std::views::keys, std::back_inserter(oldScenes));
        for (uuids::uuid id : oldScenes)
            UnloadScene(id);

        AssetBundle& assetBundle = Resources::Load(assetBundleID);

        Scene& scene = SceneAssetBundle::MoveFromAssetBundle(assetBundle, isRunning);
        allScenes.emplace(assetBundle.GetID(), &scene);

        return scene;
    }
    std::optional<std::reference_wrapper<Scene>> SceneManager::LoadScene(const std::string_view sceneName, const bool isRunning)
    {
        auto it = SceneSetting::sceneNameMapping.find(std::string(sceneName));
        if (it != SceneSetting::sceneNameMapping.end())
            return LoadScene(it->second, isRunning);
        return std::nullopt;
    }
    void SceneManager::UnloadScene(const uuids::uuid assetBundleID)
    {
        World::RemoveScene(*allScenes[assetBundleID]);
        Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
        allScenes.erase(assetBundleID);
    }

    void SceneManager_ReleaseScenes()
    {
        //引擎停止，释放场景（世界负责回收，场景需释放所有权）并回收资源包
        for (auto [id,scene] : SceneManager::allScenes)
        {
            World::RemoveScene(*scene, true);
            Resources::Unload(AssetBundle::GetAssetBundle(id));
        }
        SceneManager::allScenes.clear();
    }
}