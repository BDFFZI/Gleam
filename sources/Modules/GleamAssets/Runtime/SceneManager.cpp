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
            UnloadSceneAsync(id);

        World::PushWorldContext(prefabWorld);
        AssetBundle& assetBundle = Resources::Load(assetBundleID);
        World::PopWorldContext();

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
    void SceneManager::UnloadSceneAsync(const uuids::uuid assetBundleID)
    {
        removingScenes.emplace_back(assetBundleID);
    }

    void SceneManager_ClearAssetBundle()
    {
        //结束运行时，场景已被世界销毁，但还需清理相关资源包和场景记录
        for (const auto id : SceneManager::allScenes | std::views::keys)
            Resources::Unload(AssetBundle::GetAssetBundle(id));
        SceneManager::allScenes.clear();
    }
    void SceneManager_FlushRemovingScenes()
    {
        for (const auto assetBundleID : SceneManager::removingScenes)
        {
            World::RemoveScene(*SceneManager::allScenes[assetBundleID]);
            Resources::Unload(AssetBundle::GetAssetBundle(assetBundleID));
            SceneManager::allScenes.erase(assetBundleID);
        }
        SceneManager::removingScenes.clear();
    }
}