#include "World.h"

#define CurrentContext worldContexts.back()

namespace Gleam
{
    void World::RemoveSceneEntity(Entity& entity)
    {
        auto optionalScene = GetScene(entity);
        if (optionalScene.has_value())
            optionalScene->get().RemoveEntity(entity);

        CurrentContext->entityAllocator.RemoveEntity(entity);
    }
    void World::AddSceneSystem(const SystemInfo& systemInfo)
    {
        if (CurrentContext->activeScene != nullptr)
            CurrentContext->activeScene->AddSystem(systemInfo);
        else
            CurrentContext->systemAllocator.AddSystem(systemInfo);
    }
    void World::RemoveSceneSystem(const SystemInfo& systemInfo)
    {
        auto optionalScene = GetScene(systemInfo);
        if (optionalScene.has_value())
            optionalScene->get().RemoveSystem(systemInfo);
        else
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
        else
            scene.Destroy();

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
    std::optional<std::reference_wrapper<Scene>> World::GetScene(const SystemInfo& system)
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
        *CurrentContext = WorldContext{};
    }


    void World::FlushAsyncChange()
    {
        for (const auto& [entity,archetype] : CurrentContext->movingEntities)
            CurrentContext->entityAllocator.MoveEntity(entity, *archetype);
        CurrentContext->movingEntities.clear();

        for (auto [entity] : CurrentContext->removingEntities)
            RemoveSceneEntity(entity);
        CurrentContext->removingEntities.clear();
    }
}