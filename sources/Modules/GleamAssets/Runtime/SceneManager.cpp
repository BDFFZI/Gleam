#include "SceneManager.h"

#include "GleamEngine/Editor/Editor.h"
#include "GleamPersistence/Runtime/Resources.h"
#include "Scene/SceneAsset.h"

namespace Gleam
{
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
            if (SceneManager::scenes[id]->GetIsRunning())
                SceneManager::scenes[id]->Stop();
            SceneManager::destroyingScenes.push_back(id);
        }
        SceneManager::stoppingScenes.clear();
    }

    void SceneManager_RuntimeEvent::Start()
    {
        //将编辑器状态下添加的场景全部激活
        for (auto* scene : SceneManager::scenes | std::views::values)
        {
            if (scene->GetIsRunning() == false)
                scene->Start();
        }
    }
    void SceneManager_RuntimeEvent::Stop()
    {
        //引擎停止，释放场景（世界负责回收，场景需释放所有权）并回收资源包
        for (auto [id,scene] : SceneManager::scenes)
        {
            Scene::Destroy(*scene, true);
            Resources::Unload(AssetBundle::GetAssetBundle(id));
        }
        SceneManager::scenes.clear();
    }
}