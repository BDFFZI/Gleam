#include "Scene.h"

#include "GleamECS/Runtime/World/World.h"

namespace Gleam
{
    Scene::~Scene()
    {
        //从世界中移除托管的资源
        if (isRunning)
        {
            for (SystemInfo* system : systems)
                world->RemoveSystem(*system, false);
            isRunning = false;
        }
        for (Entity entity : entities)
            world->RemoveEntityAsync(entity);
        for (Scene* subScene : subScenes)
            world->RemoveScene(*subScene);
    }
    void Scene::Start()
    {
        for (Scene* scene : subScenes)
            scene->Start();
        for (SystemInfo* system : systems)
            world->AddSystem(*system, false);
        isRunning = true;
    }
    void Scene::Stop()
    {
        for (Scene* scene : subScenes)
            scene->Stop();
        for (SystemInfo* system : systems)
            world->RemoveSystem(*system, false);
        isRunning = false;
    }

    void Scene::AddSystem(SystemInfo& system)
    {
        assert(!systems.contains(&system) && "场景中已存在该系统！");

        systems.emplace(&system);
        world->systemToScene.emplace(&system, this);
    }
    void Scene::RemoveSystem(SystemInfo& system)
    {
        assert(systems.contains(&system) && "场景中不存在该系统！");

        systems.erase(&system);
        world->systemToScene.erase(&system);
    }
    void Scene::AddEntity(Entity entity)
    {
        assert(!entities.contains(entity) && "场景中已存在该实体！");

        entities.emplace(entity);
        world->entityToScene.emplace(entity, this);
    }
    void Scene::RemoveEntity(const Entity entity)
    {
        assert(entities.contains(entity) && "场景中不存在该实体！");

        entities.erase(entity);
        world->entityToScene.erase(entity);
    }
    void Scene::Release()
    {
        systems.clear();
        entities.clear();
        for (auto& subScene : subScenes)
            subScene->Release();
    }
}