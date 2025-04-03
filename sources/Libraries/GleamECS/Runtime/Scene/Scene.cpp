#include "Scene.h"

#include "GleamECS/Runtime/World/World.h"

namespace Gleam
{
    std::optional<std::reference_wrapper<Scene>> Scene::GetScene(std::string_view name)
    {
        auto it = std::ranges::find_if(allScenes, [name](auto& scene) { return scene->name == name; });
        if (it != allScenes.end())
            return **it;
        return std::nullopt;
    }
    std::optional<std::reference_wrapper<Scene>> Scene::GetScene(System& system)
    {
        if (auto it = systemToWorld.find(&system); it != systemToWorld.end())
            return *it->second;
        return std::nullopt;
    }
    std::optional<std::reference_wrapper<Scene>> Scene::GetScene(const Entity entity)
    {
        if (auto it = entityToWorld.find(entity); it != entityToWorld.end())
            return *it->second;
        return std::nullopt;
    }

    Scene& Scene::Create(const std::string_view name, const bool isRunning)
    {
        assert(GetScene(name) == std::nullopt && "同名场景已存在！");
        std::unique_ptr<Scene>& scene = allScenes.emplace_back(std::make_unique<Scene>());
        scene->name = name.empty() ? std::to_string(reinterpret_cast<uintptr_t>(scene.get())) : name;
        scene->isRunning = isRunning;
        return *scene;
    }
    void Scene::Destroy(Scene& scene, const bool onlyRelease)
    {
        if (!onlyRelease)
        {
            //从世界中移除托管的资源
            if (scene.isRunning)
                scene.Stop();
            for (Entity entity : scene.entities)
                World::RemoveEntityAsync(entity, false);
        }
        //移除索引信息
        for (System* system : scene.systems)
            systemToWorld.erase(system);
        for (Entity entity : scene.entities)
            entityToWorld.erase(entity);
        scene.systems.clear();
        scene.entities.clear();

        //销毁必须在Stop子场景之后，因为销毁后无法再调用子场景的函数
        for (Scene* subScene : scene.subScenes)
            Destroy(*subScene, onlyRelease);

        erase_if(allScenes, [&scene](auto& scenePtr) { return scenePtr.get() == &scene; });
    }

    void Scene::Start()
    {
        for (Scene* scene : subScenes)
            scene->Start();

        for (System* system : systems)
            World::AddSystem(*system);
        isRunning = true;
    }
    void Scene::Stop()
    {
        for (Scene* scene : subScenes)
            scene->Stop();

        for (System* system : systems)
            World::RemoveSystem(*system, false);
        isRunning = false;
    }

    void Scene::AddSystem(System& system)
    {
        assert(!systems.contains(&system) && "场景中已存在该系统！");

        systems.emplace(&system);
        systemToWorld.emplace(&system, this);
        if (isRunning)
            World::AddSystem(system);
    }
    void Scene::RemoveSystem(System& system)
    {
        assert(System::GetGlobalSystem(system.GetID()).has_value() && "场景中使用的系统必须是全局系统！");
        assert(systems.contains(&system) && "场景中不存在该系统！");

        systems.erase(&system);
        systemToWorld.erase(&system);
        if (isRunning)
            World::RemoveSystem(system, false);
    }
    void Scene::AddEntity(Entity entity)
    {
        assert(!entities.contains(entity) && "场景中已存在该实体！");

        entities.emplace(entity);
        entityToWorld.emplace(entity, this);
    }
    void Scene::RemoveEntity(const Entity entity)
    {
        assert(entities.contains(entity) && "场景中不存在该实体！");

        entities.erase(entity);
        entityToWorld.erase(entity);
    }
}