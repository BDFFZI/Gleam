#include "SceneAsset.h"

#include "EntityAsset.h"

namespace Gleam
{
    void SceneAsset::GetSceneAssets(AssetBundle& assetBundle, std::string& outName, std::vector<System*>& outSystems, std::vector<EntityAsset*>& outEntities)
    {
        SceneAsset& sceneAsset = assetBundle.GetAsset(0).GetObject<SceneAsset>();

        outName = sceneAsset.name;

        for (auto id : sceneAsset.systems)
        {
            auto optionalSystem = System::GetGlobalSystem(id);
            if (optionalSystem.has_value())
                outSystems.emplace_back(&optionalSystem.value().get());
        }

        int assetCount = assetBundle.GetAssetCount();
        for (int i = 1; i < assetCount; ++i)
        {
            EntityAsset& entityAsset = assetBundle.GetAsset(i).GetObject<EntityAsset>();
            outEntities.emplace_back(&entityAsset);
        }
    }
    void SceneAsset::SaveToAssetBundle(Scene& scene, AssetBundle& assetBundle)
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
    Scene& SceneAsset::MoveFromAssetBundle(AssetBundle& assetBundle, const bool isRunning)
    {
        std::string name;
        std::vector<System*> systems;
        std::vector<EntityAsset*> entities;
        GetSceneAssets(assetBundle, name, systems, entities);

        Scene& scene = Scene::Create(name, isRunning);
        for (System* system : systems)
            scene.AddSystem(*system);
        for (EntityAsset* entity : entities)
        {
            entity->SetOwnership(false);
            scene.AddEntity(entity->GetEntity());
        }

        return scene;
    }
    Scene& SceneAsset::CopyFromAssetBundle(AssetBundle& assetBundle, const bool isRunning)
    {
        std::string name;
        std::vector<System*> systems;
        std::vector<EntityAsset*> entities;
        GetSceneAssets(assetBundle, name, systems, entities);

        Scene& scene = Scene::Create(name + "(Clone)", isRunning);
        for (System* system : systems)
            scene.AddSystem(*system);
        for (EntityAsset* entity : entities)
        {
            Entity newEntity = World::CloneEntity(entity->GetEntity());
            scene.AddEntity(newEntity);

            //TODO 处理克隆后的引用关系重链接
        }

        return scene;
    }
}