#include "Scene.h"
#include "World.h"


namespace Gleam
{
    void Scene::Start()
    {
        for (Scene* scene : subScenes)
            scene->Start();
        for (const SystemInfo* system : systems)
            World::GetSystemAllocator().AddSystem(*system);
        isRunning = true;
    }
    void Scene::Stop()
    {
        for (Scene* scene : subScenes)
            scene->Stop();
        for (const SystemInfo* system : systems)
            World::GetSystemAllocator().RemoveSystem(*system);
        isRunning = false;
    }

    void Scene::AddSystem(const SystemInfo& system)
    {
        assert(!systems.contains(&system) && "场景中已存在该系统！");

        systems.emplace(&system);
        World::GetCurrentContext().systemToScene.emplace(&system, this);

        if (isRunning)
            World::GetSystemAllocator().AddSystem(system);
    }
    void Scene::RemoveSystem(const SystemInfo& system)
    {
        assert(systems.contains(&system) && "场景中不存在该系统！");

        if (isRunning)
            World::GetSystemAllocator().RemoveSystem(system);

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
        for (auto element : systems)
            World::GetCurrentContext().systemToScene.erase(element);
        for (auto entity : entities)
            World::GetCurrentContext().entityToScene.erase(entity);

        systems.clear();
        entities.clear();
        for (auto& subScene : subScenes)
            subScene->Release();
        subScenes.clear();
    }
    void Scene::Destroy()
    {
        for (auto element : systems)
            World::GetCurrentContext().systemToScene.erase(element);
        for (auto entity : entities)
            World::GetCurrentContext().entityToScene.erase(entity);

        //从世界中移除托管的资源
        if (isRunning)
            Stop();
        systems.clear();
        for (Entity entity : entities)
            World::GetEntityAllocator().RemoveEntity(entity); //先前已经断开了实体与场景的连接，故不会重复移除
        entities.clear();
        for (Scene* subScene : subScenes)
            World::RemoveScene(*subScene, false);
        subScenes.clear();
    }
}