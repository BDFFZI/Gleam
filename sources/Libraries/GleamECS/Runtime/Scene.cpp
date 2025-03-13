#include "Scene.h"

#include "GleamECS/Runtime/World.h"

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
    void Scene::Clear(const bool release)
    {
        if (release)
        {
            for (auto& scene : allScenes)
                scene->Release();
        }
        else
        {
            for (auto& scene : allScenes)
                scene->Reset();
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
        //移除索引信息
        for (System* system : systems)
            systemWorld.erase(system);
        for (Entity entity : entities)
            entityWorld.erase(entity);

        systems.clear();
        entities.clear();
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