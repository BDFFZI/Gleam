#include "World.h"

#include "GleamECS/Runtime/Scene.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
    void World::MoveEntityAllocator(EntityAllocator& entityAllocator)
    {
        assert(&entityAllocator.GetEntityInfoAllocator() == &entityInfoAllocator && "只有实体信息分配器相同才可融并！");
        for (auto& [archetype, heap] : entityAllocator.GetEntityHeaps())
        {
            heap.ForeachElements([archetype](std::byte* address)
            {
                Entity entity = *reinterpret_cast<Entity*>(address);
                //分配新内存
                Heap& newHeap = entities.GetEntityHeap(*archetype);
                std::byte* newAddress = newHeap.AddElement();
                //将旧数据移动到新内存
                archetype->MoveConstruct(newAddress, address);
                //设置新实体信息
                EntityInfo entityInfo = {*archetype, entities, newHeap.GetCount() - 1, newAddress};
                entityInfoAllocator.SetEntityInfo(entity, entityInfo);
            });
        }
        entityAllocator.GetEntityHeaps().clear();
    }
    void World::RemoveEntity(Entity& entity, const bool removeFromScene)
    {
        if (removeFromScene)
        {
            auto optionalScene = Scene::GetScene(entity);
            if (optionalScene.has_value())
                optionalScene->get().RemoveEntity(entity);
        }

        entities.RemoveEntity(entity);
        entity = Entity::Null; //避免野指针
    }
    void World::RemoveEntityAsync(Entity& entity, const bool removeFromScene)
    {
        removingEntities.emplace_back(entity, removeFromScene);
        entity = Entity::Null; //避免野指针
    }

    bool World::HasSystem(System& system)
    {
        return systemUsageCount.contains(&system);
    }
    void World::AddSystem(System& system)
    {
        if (system.GetGroup().has_value())
            AddSystem(system.GetGroup().value());

        const int count = ++systemUsageCount[&system];
        if (count == 1) //首次添加，需实际注册到系统组接收事件。
            system.GetGroup().value_or(systems).get().AddSubSystem(system);
    }
    void World::AddSystems(std::initializer_list<std::reference_wrapper<System>> systems)
    {
        for (System& system : systems | UnwrapRef)
            AddSystem(system);
    }
    void World::RemoveSystem(System& system, const bool removeFromScene)
    {
        if (removeFromScene)
        {
            auto optionalScene = Scene::GetScene(system);
            if (optionalScene.has_value())
            {
                optionalScene->get().RemoveSystem(system);
                return;
            }
        }

        if (system.GetGroup().has_value())
            RemoveSystem(system.GetGroup().value());

        const int count = --systemUsageCount[&system];
        if (count == 0) //首次移除，需实际从系统组中移除。
            system.GetGroup().value_or(systems).get().RemoveSubSystem(system);
        assert(count >= 0 && "重复移除系统！");
    }
    void World::RemoveSystems(std::initializer_list<std::reference_wrapper<System>> systems, const bool removeFromScene)
    {
        for (System& system : systems | UnwrapRef)
            RemoveSystem(system, removeFromScene);
    }

    Archetype& World::ComputeArchetype(
        const Entity entity,
        const std::initializer_list<std::reference_wrapper<const Type>> removingComponents,
        const std::initializer_list<std::reference_wrapper<const Type>> addingComponents)
    {
        static std::vector<std::reference_wrapper<const Type>> currentComponents = {};

        //获取已有组件
        entityInfoAllocator.GetEntityInfo(entity).archetype->GetComponentTypes(currentComponents);
        //移除目标组件
        for (std::reference_wrapper<const Type> component : removingComponents)
            std::erase_if(currentComponents, [component](auto a) { return a.get() == component.get(); });
        //添加目标组件
        currentComponents.insert(currentComponents.end(), addingComponents.begin(), addingComponents.end());

        return Archetype::CreateOrGet(currentComponents);
    }

    void World::Update()
    {
        systems.Update();
        FlushEntityQueue();
    }
    void World::Clear()
    {
        for (auto& [system,count] : systemUsageCount)
            count++; //抑制用户回收方法，防止重复回收
        systems.Stop();
        systemUsageCount.clear();

        entities.Clear();
        removingEntities.clear();
        entityInfoAllocator.Clear();
    }

    void World::FlushEntityQueue()
    {
        for (const auto& [entity,archetype] : movingEntities)
            MoveEntity(entity, *archetype);
        movingEntities.clear();

        for (auto [entity,removeFromScene] : removingEntities)
            RemoveEntity(entity, removeFromScene);
        removingEntities.clear();
    }
}