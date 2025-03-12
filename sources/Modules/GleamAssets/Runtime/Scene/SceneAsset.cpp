#include "SceneAsset.h"

#include "EntityAsset.h"

namespace Gleam
{
    void SceneAsset::ToAssetBundle(Scene& scene, AssetBundle& assetBundle)
    {
        int assetCount = static_cast<int>(assetBundle.GetAssets().size());

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
        std::vector<void*> needless;
        std::unordered_set<Entity> missing = scene.entities;
        for (int i = 1; i < assetCount; i++)
        {
            EntityAsset& entityAsset = assetBundle.GetObject<EntityAsset>(i);
            Entity entity = entityAsset.GetEntity();
            if (scene.entities.contains(entity))
                missing.erase(entity);
            else
                needless.push_back(&entityAsset);
        }
        for (void* asset : needless)
            assetBundle.RemoveAsset(asset);
        for (Entity entity : missing)
        {
            EntityAsset entityAsset = {entity, false};
            assetBundle.AddAsset(std::move(entityAsset));
        }
    }
    Scene& SceneAsset::FromAssetBundle(AssetBundle& assetBundle)
    {
        const std::vector<Asset>& assets = assetBundle.GetAssets();
        size_t assetCount = assets.size();

        //读取场景和系统信息
        SceneAsset& sceneAsset = *static_cast<SceneAsset*>(assets[0].GetObject());
        std::string_view name = sceneAsset.name;
        std::vector<System*> systems;
        for (auto id : sceneAsset.systems)
        {
            auto optionalSystem = System::GetSystem(id);
            if (optionalSystem.has_value())
                systems.emplace_back(&optionalSystem.value().get());
        }


        //读取实体信息
        std::vector<Entity> entities;
        for (std::size_t i = 1; i < assetCount; ++i)
        {
            EntityAsset& entityAsset = *static_cast<EntityAsset*>(assets[i].GetObject());
            entities.emplace_back(entityAsset.GetEntity());
            entityAsset.SetOwnership(false);
        }

        Scene& scene = Scene::Create(name);
        scene.name = name;
        for (System* system : systems)
            scene.AddSystem(*system);
        for (Entity entity : entities)
            scene.AddEntity(entity);

        return scene;
    }
}