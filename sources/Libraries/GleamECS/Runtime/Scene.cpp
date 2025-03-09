#include "Scene.h"

namespace Gleam
{
    std::optional<std::reference_wrapper<Scene>> Scene::GetScene(System& system)
    {
        if (auto it = systemWorld.find(&system); it != systemWorld.end())
            return *it->second;
        return std::nullopt;
    }
    std::optional<std::reference_wrapper<Scene>> Scene::GetScene(const Entity entity)
    {
        if (auto it = entityWorld.find(entity); it != entityWorld.end())
            return *it->second;
        return std::nullopt;
    }
    
    Scene& Scene::Create(const std::string_view name)
    {
        assert(std::ranges::count_if(allScenes,[name](auto& scene){return scene->name == name;}) ==0 && "同名场景已存在！");
            
        std::unique_ptr<Scene>& scene = allScenes.emplace_back(std::make_unique<Scene>());
        scene->name = name;
        return *scene;
    }
    void Scene::Destroy(Scene& scene)
    {
        //销毁场景
        std::erase_if(allScenes, [&scene](std::unique_ptr<Scene>& scenePtr)
        {
            return scenePtr->name == scene.name;
        });
    }
    void Scene::Clear()
    {
        allScenes.clear();
        assert(systemWorld.empty() && "场景回收异常！");
        assert(entityWorld.empty() && "场景回收异常！");
    }
    
    void Scene::ToAssetBundle(const Scene& scene, AssetBundle& assetBundle)
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
            assetBundle.GetData<SceneAsset>(0) = sceneAsset;

        //保存实体信息
        std::vector<void*> needless;
        std::unordered_set<Entity> missing = scene.entities;
        for (int i = 1; i < assetCount; i++)
        {
            EntityAsset& entityAsset = assetBundle.GetData<EntityAsset>(i);
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
            EntityAsset entityAsset = {entity};
            assetBundle.AddAsset(std::move(entityAsset));
        }
    }
    Scene& Scene::FromAssetBundle(AssetBundle& assetBundle)
    {
        const std::vector<Asset>& assets = assetBundle.GetAssets();
        size_t assetCount = assets.size();

        //读取场景和系统信息
        SceneAsset& sceneAsset = *static_cast<SceneAsset*>(assets[0].GetDataRef());
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
            EntityAsset& entityAsset = *static_cast<EntityAsset*>(assets[i].GetDataRef());
            entities.emplace_back(entityAsset.GetEntity());
        }

        Scene& scene = Create(name);
        scene.name = std::move(name);
        for (System* system : systems)
            scene.AddSystem(*system);
        for (Entity entity : entities)
            scene.AddEntity(entity);
        return scene;
    }
    
    Scene::~Scene()
    {
        if (isRunning) //从世界中移除系统
            Stop();
        for (Entity entity : entities) //从世界中移除实体
            World::RemoveEntity(entity);

        //移除索引信息
        for (Entity entity : entities)
            entityWorld.erase(entity);
        for (System* system : systems)
            systemWorld.erase(system);
    }
    void Scene::Start()
    {
        for (System* system : systems)
            World::AddSystem(*system);
        isRunning = true;
    }
    void Scene::Stop()
    {
        for (System* system : systems)
            World::RemoveSystem(*system);
        isRunning = false;
    }
    
    void Scene::AddSystem(System& system)
    {
        assert(System::GetSystem(system.GetID()).has_value() && "场景中使用的系统必须是全局系统！");
        assert(!systems.contains(&system) && "场景中已存在该系统！");

        systems.emplace(&system);
        systemWorld.emplace(&system, this);
        if (isRunning)
            World::AddSystem(system);
    }
    void Scene::RemoveSystem(System& system)
    {
        assert(System::GetSystem(system.GetID()).has_value() && "场景中使用的系统必须是全局系统！");
        assert(systems.contains(&system) && "场景中不存在该系统！");

        systems.erase(&system);
        systemWorld.erase(&system);
        if (isRunning)
            World::RemoveSystem(system);
    }
    void Scene::AddEntity(Entity entity)
    {
        assert(!entities.contains(entity) && "场景中已存在该实体！");

        entities.emplace(entity);
        entityWorld.emplace(entity, this);
    }
    void Scene::RemoveEntity(const Entity entity)
    {
        assert(entities.contains(entity) && "场景中不存在该实体！");

        entities.erase(entity);
        entityWorld.erase(entity);
    }
}