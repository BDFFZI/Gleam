#include "Scene.h"

namespace Gleam
{
    std::optional<std::reference_wrapper<EntityAsset>> EntityAsset::GetEntityAsset(const Entity entity)
    {
        return entityToAsset.contains(entity)
                   ? std::optional<std::reference_wrapper<EntityAsset>>{*entityToAsset.at(entity)}
                   : std::nullopt;
    }

    EntityAsset::EntityAsset(): entity(Entity::Null), ownership(false)
    {
    }
    EntityAsset::EntityAsset(const Entity entity, const bool ownership): entity(entity), ownership(ownership)
    {
        if (entity != Entity::Null)
            entityToAsset[entity] = this;
    }
    EntityAsset::EntityAsset(EntityAsset&& other) noexcept
    {
        entity = other.entity;
        ownership = other.ownership;

        other.entity = Entity::Null;
        other.ownership = false;

        if (entity != Entity::Null)
            entityToAsset[entity] = this;
    }
    EntityAsset& EntityAsset::operator=(EntityAsset&& other) noexcept
    {
        assert(entity != other.entity && "不能用自身移动赋值！");
        assert(other.entity != Entity::Null && "用于赋值的实体是空的！");

        if (entity == Entity::Null)
        {
            entity = other.entity;
            ownership = other.ownership;

            other.entity = Entity::Null;
            other.ownership = false;

            if (entity != Entity::Null)
                entityToAsset[entity] = this;
        }
        else if (other.entity != Entity::Null)
        {
            const EntityInfo& otherEntityInfo = World::GetEntityInfo(other.entity);
            const EntityInfo& entityInfo = World::GetEntityInfo(entity);
            World::MoveEntity(entity, *otherEntityInfo.archetype);
            otherEntityInfo.archetype->Move(entityInfo.components, otherEntityInfo.components);
        }

        return *this;
    }
    EntityAsset::~EntityAsset()
    {
        if (entity != Entity::Null)
        {
            entityToAsset.erase(entity);
            if (ownership)
                World::RemoveEntity(entity);
        }
    }

    Entity EntityAsset::GetEntity() const
    {
        return entity;
    }
    bool EntityAsset::GetOwnership() const
    {
        return ownership;
    }
    void EntityAsset::SetEntity(const Entity entity)
    {
        if (this->entity != Entity::Null)
            entityToAsset.erase(this->entity);

        this->entity = entity;

        if (this->entity != Entity::Null)
            entityToAsset[this->entity] = this;
    }
    void EntityAsset::SetOwnership(const bool ownership)
    {
        this->ownership = ownership;
    }

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
        assert(GetScene(name) == std::nullopt && "同名场景已存在！");
        std::unique_ptr<Scene>& scene = allScenes.emplace_back(std::make_unique<Scene>());
        scene->name = name;
        return *scene;
    }
    void Scene::Destroy(std::string_view name)
    {
        auto it = std::ranges::find_if(allScenes, [&name](std::unique_ptr<Scene>& scenePtr)
        {
            return scenePtr->name == name;
        });
        if (it == allScenes.end())
            throw std::runtime_error("目标场景不存在！");

        Scene& scene = **it;
        scene.Reset(); //销毁场景资源
        allScenes.erase(it);
    }
    void Scene::Destroy(Scene& scene)
    {
        Destroy(scene.name);
    }
    void Scene::Clear(const bool isReset)
    {
        if (isReset)
        {
            for (auto& scene : allScenes)
                scene->Reset();
        }
        else
        {
            for (auto& scene : allScenes)
                scene->Release();
        }

        allScenes.clear();
        assert(systemWorld.empty() && "场景回收异常！");
        assert(entityWorld.empty() && "场景回收异常！");
    }

    std::optional<std::reference_wrapper<Scene>> Scene::GetScene(std::string_view name)
    {
        auto it = std::ranges::find_if(allScenes, [name](auto& scene) { return scene->name == name; });
        if (it != allScenes.end())
            return **it;
        return std::nullopt;
    }

    void Scene::ToAssetBundle(Scene& scene, AssetBundle& assetBundle)
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
    Scene& Scene::FromAssetBundle(AssetBundle& assetBundle)
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

        Scene& scene = Create(name);
        scene.name = name;
        for (System* system : systems)
            scene.AddSystem(*system);
        for (Entity entity : entities)
            scene.AddEntity(entity);

        return scene;
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
    void Scene::Release()
    {
        systems.clear();
        entities.clear();
        //移除索引信息
        for (System* system : systems)
            systemWorld.erase(system);
        for (Entity entity : entities)
            entityWorld.erase(entity);
    }
    void Scene::Reset()
    {
        if (isRunning) //从世界中移除系统
            Stop();
        for (Entity entity : entities) //从世界中移除实体
            World::RemoveEntity(entity);
        Release();
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