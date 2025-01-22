#include "World.h"

#include <ranges>

namespace Light
{
    EntityInfo Scene::GetEntityInfo(const Entity entity)
    {
        return entityInfos.at(entity);
    }

    bool Scene::HasEntity(const Entity entity)
    {
        return entityInfos.contains(entity);
    }

    Entity World::CreateEntity()
    {
        return static_cast<Entity>(nextEntity++);
    }
    /**
     * @brief 添加系统
     *
     * 1. 会自动递归添加依赖的系统组
     * 2. 允许重复添加，会自动记录使用计数以供移除时使用
     * 
     * @param system 
     */
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
    /**
     * @brief 移除系统
     *
     * 1. 会自动递归移除依赖的系统组
     * 2. 重复添加后需重复移除，当使用计数为0时才会真正移除系统
     * 
     * @param system 
     */
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

    void World::RemoveHeapItem(const Archetype& archetype, const int index)
    {
        Heap& heap = GetEntityHeap(archetype);
        std::byte* element = heap.RemoveElement(index);
        //删除时末尾项会被用来替补空位，所以相关实体信息也需要更变
        const Entity movedEntity = *reinterpret_cast<Entity*>(element);
        EntityInfo& movedEntityInfo = entityInfos[movedEntity];
        movedEntityInfo.components = element;
        movedEntityInfo.indexAtHeap = index;
    }
}