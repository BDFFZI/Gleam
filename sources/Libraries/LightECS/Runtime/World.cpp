#include "World.h"

#include <ranges>

namespace Light
{
    EntityInfo::EntityInfo(Scene* scene, const Archetype* archetype, const int indexAtHeap, std::byte* components)
        : scene(scene), archetype(archetype), indexAtHeap(indexAtHeap), components(components)
    {
    }
    EntityInfo::EntityInfo(Scene* scene, const Archetype* archetype, const int indexAtHeap)
        : scene(scene), archetype(archetype), indexAtHeap(indexAtHeap),
          components(scene->GetEntityHeap(archetype).At(indexAtHeap))
    {
    }
    bool World::HasEntity(const Entity entity)
    {
        return entityInfos.contains(entity);
    }
    Entity World::GetNextEntity()
    {
        return static_cast<Entity>(nextEntity++);
    }
    EntityInfo& World::GetEntityInfo(const Entity entity)
    {
        return entityInfos.at(entity);
    }
    void World::SetEntityInfo(const Entity entity, const std::optional<EntityInfo>& info)
    {
        if (info.has_value())
            entityInfos[entity] = info.value();
        else
            entityInfos.erase(entity);
    }

    const std::vector<std::unique_ptr<Scene>>& World::GetAllScenes()
    {
        return allScenes;
    }
    Scene* World::CreateScene(const std::string_view name)
    {
        return allScenes.emplace_back(new Scene(name)).get();
    }
    Scene* World::GetMainScene()
    {
        return mainScene;
    }
    Entity World::AddEntity(const Archetype* archetype, Scene* scene)
    {
        return scene->AddEntity(archetype);
    }
    void World::AddEntities(const Archetype* archetype, const int count, Entity* outEntities, Scene* scene)
    {
        scene->AddEntities(archetype, count, outEntities);
    }
    void World::RemoveEntity(Entity& entity)
    {
        EntityInfo& entityInfo = GetEntityInfo(entity);
        entityInfo.scene->RemoveEntity(entity);
    }
    void World::MoveEntity(const Entity entity, const Archetype* newArchetype)
    {
        EntityInfo& entityInfo = GetEntityInfo(entity);
        entityInfo.scene->MoveEntity(entity, newArchetype);
    }
    void World::MoveEntitySimply(const Entity entity, const Archetype* newArchetype)
    {
        EntityInfo& entityInfo = GetEntityInfo(entity);
        entityInfo.scene->MoveEntitySimply(entity, newArchetype);
    }
    void World::MoveEntity(const Entity entity, Scene* newScene)
    {
        EntityInfo& entityInfo = GetEntityInfo(entity);
        entityInfo.scene->MoveEntity(entity, newScene);
    }

    bool World::HasSystem(System& system)
    {
        return systemUsageCount.contains(&system);
    }
    void World::AddSystem(System* system)
    {
        if (system->GetGroup().has_value())
            AddSystem(system->GetGroup().value());

        const int count = 1 + (systemUsageCount.contains(system) ? systemUsageCount[system] : 0);
        systemUsageCount[system] = count;

        if (count == 1)
        {
            //首次添加，需实际注册到系统组接收事件。
            system->GetGroup().value_or(&allSystems)->AddSubSystem(system);
        }
    }
    void World::AddSystems(const std::initializer_list<System*> systems)
    {
        for (auto system : systems)
            AddSystem(system);
    }
    void World::RemoveSystem(System* system)
    {
        assert(systemUsageCount.contains(system) && "无法移除未添加过的系统！");

        if (system->GetGroup().has_value())
            RemoveSystem(system->GetGroup().value());

        const int count = systemUsageCount[system] - 1;
        if (count == 0)
            systemUsageCount.erase(system);
        else
            systemUsageCount[system] = count;

        if (count == 0)
        {
            //最后一次移除，需实际从系统组移除。
            system->GetGroup().value_or(&allSystems)->RemoveSubSystem(system);
        }
    }
    void World::RemoveSystems(const std::initializer_list<System*> systems)
    {
        for (auto system : systems)
            RemoveSystem(system);
    }

    void World::Start()
    {
        allSystems.Start();
    }
    void World::Stop()
    {
        allSystems.Stop();
    }
    void World::Update()
    {
        allSystems.Update();
    }
}