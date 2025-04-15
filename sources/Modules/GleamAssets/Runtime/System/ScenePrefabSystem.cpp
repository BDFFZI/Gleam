#include "ScenePrefabSystem.h"

#include "GleamAssets/Runtime/SceneAssetBundle.h"
#include "GleamAssets/Runtime/Entity/ScenePrefab.h"

namespace Gleam
{
    void ScenePrefabSystem::Update()
    {
        World::GetView<ScenePrefab>().Each([](const Entity entity, ScenePrefab& scenePrefab)
        {
            if (scenePrefab.sourceScene.expired())
                return;
            if (scenePrefab.instanceScene != nullptr)
                return;
            std::optional<std::reference_wrapper<Scene>> rootScene = World::GetScene(entity);
            if (!rootScene.has_value()) //暂不支持在非场景中使用ScenePrefab，因为依赖SceneManager对场景的内存管理功能，以及场景的系统开关功能。
                return;

            AssetRef assetRef = AssetBundle::GetAssetRef(scenePrefab.sourceScene).value();
            AssetBundle& assetBundle = AssetBundle::GetAssetBundle(assetRef.assetBundleID);
            scenePrefab.instanceScene = &SceneAssetBundle::CopyFromAssetBundle(assetBundle, rootScene->get().GetIsRunning());
            rootScene->get().AddSubScene(*scenePrefab.instanceScene);
        });
    }
}