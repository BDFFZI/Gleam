#include "SceneAsset.h"

#include "EntityAsset.h"

namespace Gleam
{
    void SceneAsset::ToAssetBundle(Scene& scene, AssetBundle& assetBundle)
    {
        int assetCount = static_cast<int>(assetBundle.GetAssetSlots().size());

        //保存场景和系统信息
        SceneAsset sceneAsset;
        sceneAsset.name = scene.name;
        for (System* system : scene.systems)
            sceneAsset.systems.push_back(system->GetID());
        if (assetCount == 0)
            assetBundle.AddAsset(std::move(sceneAsset));
        else
            assetBundle.GetObject<SceneAsset>(0) = sceneAsset;

        //保存实体信息
        std::vector<std::shared_ptr<void>> needless;
        std::unordered_set<Entity> missing = scene.entities;
        for (int i = 1; i < assetCount; i++)
        {
            EntityAsset& entityAsset = assetBundle.GetObject<EntityAsset>(i);
            Entity entity = entityAsset.GetEntity();
            if (scene.entities.contains(entity))
                missing.erase(entity);
            else
                needless.push_back(assetBundle.GetAsset(i).GetObjectPtr());
        }
        //去除资源包中多余的
        for (const std::shared_ptr<void>& asset : needless)
            assetBundle.RemoveAsset(asset);
        //添加资源包中缺少的
        for (Entity entity : missing)
        {
            EntityAsset entityAsset = {entity, false};
            assetBundle.AddAsset(std::move(entityAsset));
        }
    }
    Scene& SceneAsset::FromAssetBundle(AssetBundle& assetBundle, const bool isRunning)
    {
        //读取场景和系统信息
        SceneAsset& sceneAsset = assetBundle.GetAsset(0).GetObject<SceneAsset>();
        std::string_view name = sceneAsset.name;
        std::vector<System*> systems;
        for (auto id : sceneAsset.systems)
        {
            auto optionalSystem = System::GetGlobalSystem(id);
            if (optionalSystem.has_value())
                systems.emplace_back(&optionalSystem.value().get());
        }


        //读取实体信息
        std::vector<Entity> entities;
        size_t assetCount = assetBundle.GetAssetCount();
        for (std::size_t i = 1; i < assetCount; ++i)
        {
            EntityAsset& entityAsset = assetBundle.GetAsset(i).GetObject<EntityAsset>();
            entities.emplace_back(entityAsset.GetEntity());
            entityAsset.SetOwnership(false);
        }

        Scene& scene = Scene::Create(name, isRunning);
        scene.name = name;
        for (System* system : systems)
            scene.AddSystem(*system);
        for (Entity entity : entities)
            scene.AddEntity(entity);

        return scene;
    }
}