#include "World.h"

#define CurrentContext worldContexts.back()

namespace Gleam
{
    Entity World::AddEntity(const Archetype& archetype, const bool addToScene)
    {
        Entity entity = CurrentContext->entityAllocator.AddEntity(archetype);

        if (addToScene && CurrentContext->activeScene != nullptr)
            CurrentContext->activeScene->AddEntity(entity);

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

        CurrentContext->entityAllocator.RemoveEntity(entity);
    }
    IOrderedSystemEvent& World::AddSystem(const SystemInfo& systemInfo, const bool addToScene)
    {
        if (addToScene && CurrentContext->activeScene != nullptr)
            CurrentContext->activeScene->AddSystem(systemInfo);

        return CurrentContext->systemAllocator.AddSystem(systemInfo);
    }
    void World::RemoveSystem(const SystemInfo& systemInfo, const bool removeFromScene)
    {
        if (removeFromScene && CurrentContext->activeScene != nullptr)
            CurrentContext->activeScene->RemoveSystem(systemInfo);

        CurrentContext->systemAllocator.RemoveSystem(systemInfo);
    }
    Scene& World::AddScene(const std::string_view name, const bool isRunning)
    {
        assert(GetScene(name) == std::nullopt && "同名场景已存在！");
        std::unique_ptr<Scene>& scene = CurrentContext->allScenes.emplace_back(std::make_unique<Scene>(
            name, isRunning
        ));
        CurrentContext->activeScene = scene.get();
        return *scene;
    }
    void World::RemoveScene(Scene& scene, const bool release)
    {
        if (release)
            scene.Release();
        if (CurrentContext->activeScene == &scene)
            CurrentContext->activeScene = nullptr;
        erase_if(CurrentContext->allScenes, [&scene](auto& scenePtr) { return scenePtr.get() == &scene; });
    }
    std::optional<std::reference_wrapper<Scene>> World::GetScene(std::string_view name)
    {
        auto it = std::ranges::find_if(CurrentContext->allScenes, [name](std::unique_ptr<Scene>& scene) { return scene->GetName() == name; });
        return it != CurrentContext->allScenes.end() ? std::optional<std::reference_wrapper<Scene>>(**it) : std::nullopt;
    }
    std::optional<std::reference_wrapper<Scene>> World::GetScene(const Entity entity)
    {
        if (auto it = CurrentContext->entityToScene.find(entity); it != CurrentContext->entityToScene.end())
            return *it->second;
        return std::nullopt;
    }
    std::optional<std::reference_wrapper<Scene>> World::GetScene(SystemInfo& system)
    {
        if (auto it = CurrentContext->systemToScene.find(&system); it != CurrentContext->systemToScene.end())
            return *it->second;
        return std::nullopt;
    }

    void World::Update()
    {
        CurrentContext->systemAllocator.Update();
        FlushAsyncChange();
    }
    void World::Clear()
    {
        *CurrentContext = {};
    }


    void World::FlushAsyncChange()
    {
        for (const auto& [entity,archetype] : CurrentContext->movingEntities)
            CurrentContext->entityAllocator.MoveEntity(entity, *archetype);
        CurrentContext->movingEntities.clear();

        for (auto [entity,removeFromScene] : CurrentContext->removingEntities)
            RemoveEntity(entity, removeFromScene);
        CurrentContext->removingEntities.clear();
    }
}