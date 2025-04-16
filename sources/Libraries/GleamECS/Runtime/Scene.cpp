#include "Scene.h"
#include "World.h"


namespace Gleam
{
    void Scene::Start()
    {
        for (Scene* scene : subScenes)
            scene->Start();
        for (const SystemInfo* system : systems)
            World::AddSystem(*system, false);
        isRunning = true;
    }
    void Scene::Stop()
    {
        for (Scene* scene : subScenes)
            scene->Stop();
        for (const SystemInfo* system : systems)
            World::RemoveSystem(*system, false);
        isRunning = false;
    }

    void Scene::AddSystem(const SystemInfo& system)
    {
        assert(!systems.contains(&system) && "场景中已存在该系统！");

        systems.emplace(&system);
        World::GetCurrentContext().systemToScene.emplace(&system, this);
    }
    void Scene::RemoveSystem(const SystemInfo& system)
    {
        assert(systems.contains(&system) && "场景中不存在该系统！");

        systems.erase(&system);
        World::GetCurrentContext().systemToScene.erase(&system);
    }
    void Scene::AddEntity(Entity entity)
    {
        assert(!entities.contains(entity) && "场景中已存在该实体！");

        entities.emplace(entity);
        World::GetCurrentContext().entityToScene.emplace(entity, this);
    }
    void Scene::RemoveEntity(const Entity entity)
    {
        assert(entities.contains(entity) && "场景中不存在该实体！");

        entities.erase(entity);
        World::GetCurrentContext().entityToScene.erase(entity);
    }
    void Scene::Release()
    {
        systems.clear();
        entities.clear();
        for (auto& subScene : subScenes)
            subScene->Release();
    }
    void Scene::Destroy()
    {
        //从世界中移除托管的资源
        if (isRunning)
        {
            for (const SystemInfo* system : systems)
                World::RemoveSystem(*system, false);
            isRunning = false;
        }
        for (Entity entity : entities)
            World::RemoveEntityAsync(entity, false);
        for (Scene* subScene : subScenes)
            World::RemoveScene(*subScene, false);
    }
}