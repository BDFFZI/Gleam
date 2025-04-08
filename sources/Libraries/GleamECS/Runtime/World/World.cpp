#include "World.h"

namespace Gleam
{
    Entity World::AddEntity(const Archetype& archetype, const bool addToScene)
    {
        Entity entity = entityAllocator.AddEntity(archetype);

        if (addToScene && activeScene != nullptr)
            activeScene->AddEntity(entity);

        return entity;
    }
    void World::RemoveEntity(Entity& entity, const bool removeFromScene)
    {
        if (removeFromScene)
        {
            auto optionalScene = GetScene(entity);
            if (optionalScene.has_value())
                optionalScene->get().RemoveEntity(entity);
        }

        entityAllocator.RemoveEntity(entity);
    }
    System& World::AddSystem(SystemInfo& systemInfo, const bool addToScene)
    {
        if (addToScene && activeScene != nullptr)
            activeScene->AddSystem(systemInfo);

        return systemAllocator.AddSystem(systemInfo);
    }
    void World::RemoveSystem(SystemInfo& systemInfo, const bool removeFromScene)
    {
        if (removeFromScene && activeScene != nullptr)
            activeScene->RemoveSystem(systemInfo);

        systemAllocator.RemoveSystem(systemInfo);
    }
    Scene& World::AddScene(const std::string_view name, const bool isRunning)
    {
        assert(GetScene(name) == std::nullopt && "同名场景已存在！");
        std::unique_ptr<Scene>& scene = allScenes.emplace_back(std::make_unique<Scene>(
            *this, name, isRunning
        ));
        activeScene = scene.get();
        return *scene;
    }
    void World::RemoveScene(Scene& scene, const bool release)
    {
        if (release)
            scene.Release();
        if (activeScene == &scene)
            activeScene = nullptr;
        erase_if(allScenes, [&scene](auto& scenePtr) { return scenePtr.get() == &scene; });
    }

    void World::Update()
    {
        currentWorld = this;
        systemAllocator.Update();
        FlushAsyncChange();
        currentWorld = nullptr;
    }
    void World::Clear()
    {
        currentWorld = this;
        entityAllocator.Clear();
        entityInfoAllocator.Clear();
        removingEntities.clear();
        systemAllocator.Clear();
        currentWorld = nullptr;
    }

    void World::FlushAsyncChange()
    {
        for (const auto& [entity,archetype] : movingEntities)
            entityAllocator.MoveEntity(entity, *archetype);
        movingEntities.clear();

        for (auto [entity,removeFromScene] : removingEntities)
            RemoveEntity(entity, removeFromScene);
        removingEntities.clear();
    }
}