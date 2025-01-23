#include "World.h"

#include <ranges>

namespace Light
{
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

    Scene& World::CreateScene(const std::string_view name)
    {
        return *allScenes.emplace_back(new Scene(name));
    }
    Scene& World::GetOrCreateScene(const std::string_view name)
    {
        auto result = std::ranges::find_if(
            allScenes,
            [name](const std::unique_ptr<Scene>& scene) { return scene->GetName() == name; }
        );
        if (result == allScenes.end())
            return CreateScene(name);
        return **result;
    }
    Scene& World::GetDefaultScene()
    {
        return defaultScene;
    }
    std::vector<std::reference_wrapper<Scene>>& World::GetVisibleScenes()
    {
        return visibleScenes;
    }

    bool World::HasSystem(System& system)
    {
        return systemUsageCount.contains(&system);
    }
    void World::AddSystem(System& system)
    {
        if (system.group != nullptr)
            AddSystem(*system.group);

        const int count = 1 + (systemUsageCount.contains(&system) ? systemUsageCount[&system] : 0);
        systemUsageCount[&system] = count;

        if (count == 1)
        {
            //首次添加，需实际注册到系统组接收事件。
            if (system.group == nullptr)
                allSystems.AddSubSystem(system);
            else
                system.group->AddSubSystem(system);
        }
    }
    void World::AddSystems(const std::initializer_list<System*> systems)
    {
        for (System* system : systems)
            AddSystem(*system);
    }
    void World::RemoveSystem(System& system)
    {
        assert(systemUsageCount.contains(&system) && "无法移除未添加过的系统！");

        if (system.group != nullptr)
            RemoveSystem(*system.group);

        const int count = systemUsageCount[&system] - 1;
        if (count == 0)
            systemUsageCount.erase(&system);
        else
            systemUsageCount[&system] = count;

        if (count == 0)
        {
            //最后一次移除，需实际从系统组移除。
            if (system.group == nullptr)
                allSystems.RemoveSubSystem(system);
            else
                system.group->RemoveSubSystem(system);
        }
    }
    void World::RemoveSystems(const std::initializer_list<System*> systems)
    {
        for (System* system : systems)
            RemoveSystem(*system);
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