#include "World.h"

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

    Entity World::GetNextEntity()
    {
        return static_cast<Entity>(nextEntity++);
    }
    const EntityInfo& World::GetEntityInfo(const Entity entity)
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

    Scene* World::GetMainScene()
    {
        return &mainScene;
    }
    SystemGroup* World::GetMainSystem()
    {
        return &mainSystem;
    }

    bool World::HasEntity(const Entity entity)
    {
        return entityInfos.contains(entity);
    }
    Entity World::AddEntity(const Archetype* archetype)
    {
        return mainScene.AddEntity(archetype);
    }
    void World::RemoveEntity(Entity& entity)
    {
        mainScene.RemoveEntity(entity);
    }
    void World::MoveEntity(const Entity entity, const Archetype* newArchetype)
    {
        const EntityInfo& entityInfo = GetEntityInfo(entity);
        entityInfo.scene->MoveEntity(entity, newArchetype);
    }
    void World::MoveEntitySimply(const Entity entity, const Archetype* newArchetype)
    {
        const EntityInfo& entityInfo = GetEntityInfo(entity);
        entityInfo.scene->MoveEntitySimply(entity, newArchetype);
    }


    bool World::HasSystem(System& system)
    {
        return systemUsageCount.contains(&system);
    }
    void World::AddSystem(System* system)
    {
        if (system->GetGroup().has_value())
            AddSystem(system->GetGroup().value());
        addingSystems.push_back(system);
    }
    void World::AddSystems(const std::initializer_list<System*> systems)
    {
        for (System* system : systems)
            AddSystem(system);
    }
    void World::RemoveSystem(System* system)
    {
        if (system->GetGroup().has_value())
            RemoveSystem(system->GetGroup().value());
        removingSystems.push_back(system);
    }
    void World::RemoveSystems(const std::initializer_list<System*> systems)
    {
        for (System* system : systems)
            RemoveSystem(system);
    }

    void World::Start()
    {
        FlushSystemQueue(); //将所有系统送到开始队列，实体送到Awake队列
        mainSystem.Start(); //执行开始队列并将系统转移到更新队列
    }
    void World::Stop()
    {
        mainScene.RemoveAllEntities(); //移除所有实体
        mainSystem.Stop(); //停止所有队列，忽略开始队列
    }
    void World::Update(const bool negative)
    {
        //将更新过程中标记增删的实体或系统移入到对应的事件队列
        FlushSystemQueue();
        //运行更新队列
        if (negative)
            mainSystem.UpdateNegative();
        else
            mainSystem.Update();
    }

    void World::FlushSystemQueue()
    {
        for (auto system : removingSystems)
        {
            assert(systemUsageCount.contains(system) && "无法移除未添加过的系统！");

            const int count = systemUsageCount[system] - 1;
            if (count == 0)
                systemUsageCount.erase(system);
            else
                systemUsageCount[system] = count;

            if (count == 0)
            {
                //最后一次移除，需实际从系统组移除。
                system->GetGroup().value_or(&mainSystem)->RemoveSubSystem(system);
            }
        }
        removingSystems.clear();
        for (auto system : addingSystems)
        {
            const int count = 1 + (systemUsageCount.contains(system) ? systemUsageCount[system] : 0);
            systemUsageCount[system] = count;

            if (count == 1)
            {
                //首次添加，需实际注册到系统组接收事件。
                system->GetGroup().value_or(&mainSystem)->AddSubSystem(system);
            }
        }
        addingSystems.clear();
    }
}