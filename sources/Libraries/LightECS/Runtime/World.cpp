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
        return updateScene.get();
    }
    Scene* World::GetAwakeScene()
    {
        return awakeScene.get();
    }
    Scene* World::GetDestroyScene()
    {
        return destroyScene.get();
    }

    bool World::HasEntity(const Entity entity)
    {
        return entityInfos.contains(entity);
    }
    Entity World::AddEntity(const Archetype* archetype)
    {
        Entity entity = readyScene->AddEntity(archetype);
        movingEntities.emplace_back(entity, awakeScene.get());
        return entity;
    }
    void World::RemoveEntity(Entity& entity)
    {
        movingEntities.emplace_back(entity, destroyScene.get());
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
        FlushBufferQueue(); //将所有系统送到开始队列，实体送到Awake队列
        allSystems.Start(); //执行开始队列并将系统转移到更新队列
    }
    void World::Stop()
    {
        allSystems.Stop(); //停止所有队列，忽略开始队列
        //所有事件均停止并移除，不能再次触发FlushSystemQueue，不然可能导致重复移除
    }
    void World::Update()
    {
        //将更新过程中标记增删的实体或系统移入到对应的事件队列
        FlushBufferQueue();
        //运行更新队列
        allSystems.Update();
        //清理事件队列中的实体
        Scene::MoveAllEntities(awakeScene.get(), updateScene.get()); //唤醒事件结束，相关实体移入主场景
        destroyScene->RemoveAllEntities(); //删除事件结束，将相关实体真正从内存中移除
    }
    void World::Pause()
    {
        allSystems.SetIsRunning(false);
    }
    void World::Play()
    {
        allSystems.SetIsRunning(true);
    }
    void World::FlushBufferQueue()
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
                system->GetGroup().value_or(&allSystems)->RemoveSubSystem(system);
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
                system->GetGroup().value_or(&allSystems)->AddSubSystem(system);
            }
        }
        addingSystems.clear();
        for (const auto& [entity,scene] : movingEntities)
            Scene::MoveEntity(entity, scene);
        movingEntities.clear();
    }
}