#include "SceneAssetBundle.h"
#include "Asset/BasicSceneInfo.h"

namespace Gleam
{
    void SceneAssetBundle::GetSceneAssets(AssetBundle& assetBundle, std::string& outName, std::vector<const SystemInfo*>& outSystems, std::vector<PersistentEntity*>& outEntities)
    {
        BasicSceneInfo& sceneAsset = assetBundle.GetAsset(0).GetObject<BasicSceneInfo>();

        outName = sceneAsset.name;

        for (auto id : sceneAsset.systems)
        {
            if (SystemInfoAllocator::HasSystemInfo(id))
                outSystems.emplace_back(&SystemInfoAllocator::GetSystemInfo(id));
        }

        int assetCount = assetBundle.GetAssetCount();
        for (int i = 1; i < assetCount; ++i)
        {
            PersistentEntity& entityAsset = assetBundle.GetAsset(i).GetObject<PersistentEntity>();
            outEntities.emplace_back(&entityAsset);
        }
    }
    void SceneAssetBundle::SaveToAssetBundle(Scene& scene, AssetBundle& assetBundle)
    {
        int assetCount = static_cast<int>(assetBundle.GetAssetSlots().size());

        //保存场景和系统信息
        BasicSceneInfo sceneAsset;
        sceneAsset.name = scene.GetName();
        for (const SystemInfo* system : scene.GetSystems())
            sceneAsset.systems.push_back(system->type->GetID());
        if (assetCount == 0)
            assetBundle.AddAsset(std::move(sceneAsset));
        else
            assetBundle.GetObject<BasicSceneInfo>(0) = sceneAsset;

        //保存实体信息
        std::vector<std::shared_ptr<void>> needless;
        std::unordered_set<Entity> missing = scene.GetEntities();
        for (int i = 1; i < assetCount; i++)
        {
            PersistentEntity& entityAsset = assetBundle.GetObject<PersistentEntity>(i);
            Entity entity = entityAsset.GetEntity();
            if (scene.GetEntities().contains(entity))
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
            PersistentEntity entityAsset = {entity, false};
            assetBundle.AddAsset(std::move(entityAsset));
        }
    }
    Scene& SceneAssetBundle::MoveFromAssetBundle(AssetBundle& assetBundle, const bool isRunning)
    {
        std::string name;
        std::vector<System*> systems;
        std::vector<PersistentEntity*> entities;
        GetSceneAssets(assetBundle, name, systems, entities);

        Scene& scene = World::AddScene(name, isRunning);
        for (const SystemInfo* system : systems)
            scene.AddSystem(*system);
        for (PersistentEntity* entity : entities)
        {
            entity->SetOwnership(false);
            scene.AddEntity(entity->GetEntity());
        }

        return scene;
    }
    Scene& SceneAssetBundle::CopyFromAssetBundle(AssetBundle& assetBundle, const bool isRunning)
    {
        std::string name;
        std::vector<System*> systems;
        std::vector<PersistentEntity*> entities;
        GetSceneAssets(assetBundle, name, systems, entities);

        Scene& scene = World::AddScene(name + "(Clone)", isRunning);
        for (const SystemInfo* system : systems)
            scene.AddSystem(*system);
        for (PersistentEntity* entity : entities)
        {
            Entity newEntity = World::GetEntityAllocator().CloneEntity(entity->GetEntity());
            scene.AddEntity(newEntity);

            //TODO 处理克隆后的引用关系重链接
        }

        return scene;
    }
}