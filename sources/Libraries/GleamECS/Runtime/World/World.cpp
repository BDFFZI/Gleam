#include "World.h"

#include "GleamECS/Runtime/Scene.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
    bool World::HasEntity(const Entity entity)
    {
        return entityInfoAllocator.HasEntity(entity);
    }
    Entity World::AddEntity(const Archetype& archetype)
    {
        return addingEntities.AddEntity(archetype);
    }
    void World::RemoveEntity(Entity& entity, const bool removeFromScene)
    {
        if (removeFromScene)
        {
            auto optionalScene = Scene::GetScene(entity);
            if (optionalScene.has_value())
                optionalScene->get().RemoveEntity(entity);
        }

        if (entityInfoAllocator.GetEntityInfo(entity).allocator == &addingEntities)
        {
            addingEntities.RemoveEntity(entity);
            return;
        }

        removingEntities.emplace_back(entity);
        entity = Entity::Null; //避免野指针
    }
    void World::MoveEntity(const Entity entity, const Archetype& newArchetype)
    {
        const EntityInfo& entityInfo = entityInfoAllocator.GetEntityInfo(entity);
        entityInfo.allocator->MoveEntity(entity, newArchetype);
    }

    bool World::HasSystem(System& system)
    {
        return systemUsageCount.contains(&system);
    }
    void World::AddSystem(System& system)
    {
        if (system.GetGroup().has_value())
            AddSystem(system.GetGroup().value());

        if (auto it = removingSystems.find(&system); it != removingSystems.end())
        {
            removingSystems.erase(it); //优先使用removingSystems抵消，该功能用于实现两个场景共用系统的情况
            return;
        }

        addingSystems.emplace(&system);
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

        if (auto it = addingSystems.find(&system); it != addingSystems.end())
        {
            addingSystems.erase(it); //优先使用addingSystems抵消，该功能用于实现编辑器模式下拦截用户系统
            return;
        }

        removingSystems.emplace(&system);
    }
    void World::RemoveSystems(std::initializer_list<std::reference_wrapper<System>> systems, const bool removeFromScene)
    {
        for (System& system : systems | UnwrapRef)
            RemoveSystem(system, removeFromScene);
    }

    void World::AddComponents(const Entity entity, const std::initializer_list<std::reference_wrapper<const Type>> componentTypes)
    {
        static std::vector<std::reference_wrapper<const Type>> currentComponents = {};

        entityInfoAllocator.GetEntityInfo(entity).archetype->GetComponentTypes(currentComponents);
        currentComponents.insert(currentComponents.end(), componentTypes.begin(), componentTypes.end());

        Archetype& archetype = Archetype::CreateOrGet(currentComponents);
        MoveEntity(entity, archetype);
    }
    void World::RemoveComponents(const Entity entity, const std::initializer_list<std::reference_wrapper<const Type>> componentTypes)
    {
        static std::vector<std::reference_wrapper<const Type>> currentComponents = {};

        entityInfoAllocator.GetEntityInfo(entity).archetype->GetComponentTypes(currentComponents);
        for (std::reference_wrapper<const Type> component : componentTypes)
            std::erase_if(currentComponents, [component](auto a) { return a.get() == component.get(); });

        Archetype& archetype = Archetype::CreateOrGet(currentComponents);
        MoveEntity(entity, archetype);
    }

    void World::Update()
    {
        FlushSystemQueue(); //应用上一帧对系统的结构性更改
        systems.FlushStopQueue(); //当前帧停止的事件对当前帧销毁的实体可见，以便进行回收工作，因此先于实体销毁前执行。
        FlushEntityQueue(); //应用上一帧对实体的结构性更改
        //当前帧的开始和更新事件无法处理当前帧被销毁的实体。
        systems.FlushStartQueue();
        systems.Update();
    }
    void World::Clear()
    {
        systems.Stop();
        addingSystems.clear();
        removingSystems.clear();
        systemUsageCount.clear();

        entities.Clear();
        addingEntities.Clear();
        removingEntities.clear();
        entityInfoAllocator.Clear();
    }

    void World::FlushSystemQueue()
    {
        for (System* system : addingSystems)
        {
            const int count = ++systemUsageCount[system];
            if (count == 1) //首次添加，需实际注册到系统组接收事件。
                system->GetGroup().value_or(systems).get().AddSubSystem(*system);
        }
        addingSystems.clear();
        for (System* system : removingSystems)
        {
            const int count = --systemUsageCount[system];
            if (count == 0) //首次添加，需实际注册到系统组接收事件。
                system->GetGroup().value_or(systems).get().RemoveSubSystem(*system);
            assert(count >= 0 && "重复移除系统！");
        }
        removingSystems.clear();
    }
    void World::FlushEntityQueue()
    {
        for (auto entity : removingEntities)
            entities.RemoveEntity(entity);
        removingEntities.clear();

        for (auto& [archetype, heap] : addingEntities.GetEntityHeaps())
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
        addingEntities.GetEntityHeaps().clear();
    }
}