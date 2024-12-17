#include "World.h"

#include <ranges>

namespace Light
{
    EntityInfo World::GetEntityInfo(const Entity entity)
    {
        return entityInfos.at(entity);
    }

    bool World::HasEntity(const Entity entity)
    {
        return entityInfos.contains(entity);
    }
    Entity World::AddEntity(const Archetype& archetype)
    {
        Heap& heap = GetEntities(archetype);
        int startIndex = heap.GetCount();
        Entity entity = static_cast<Entity>(nextEntity++);
        heap.AddElement([&archetype,startIndex,entity](std::byte* item)
        {
            archetype.RunConstructor(item);
            *reinterpret_cast<Entity*>(item) = entity;
            entityInfos.insert({entity, {&archetype, item, startIndex}});
        });

        return entity;
    }
    void World::AddEntities(const Archetype& archetype, const int count, Entity* outEntities)
    {
        Heap& heap = GetEntities(archetype);
        int startIndex = heap.GetCount();
        heap.AddElements(count, [&archetype,outEntities,startIndex](const int itemIndex, std::byte* item)
        {
            Entity entity = static_cast<Entity>(nextEntity++);
            archetype.RunConstructor(item);
            *reinterpret_cast<Entity*>(item) = entity;
            entityInfos.insert({entity, {&archetype, item, startIndex + itemIndex}});
            if (outEntities != nullptr)outEntities[itemIndex] = entity;
        });
    }
    void World::MoveEntity(const Entity entity, const Archetype& newArchetype)
    {
        assert(entity != Entity::Null && "目标实体为空！");
        assert(entityInfos.contains(entity) && "目标实体不存在！");

        //获取旧实体信息
        EntityInfo entityInfo = entityInfos[entity];
        const Archetype& oldArchetype = *entityInfo.archetype;
        //分配新内存并复制原数据
        Heap& newHeap = GetEntities(newArchetype);
        newHeap.AddElement([&entityInfo,oldArchetype,newArchetype](std::byte* address)
        {
            for (int i = 0; i < newArchetype.componentCount; ++i)
            {
                //遍历每个新原形的组件
                std::type_index type = newArchetype.componentTypes[i];
                const size_t size = newArchetype.componentSizes[i];
                if (oldArchetype.HasComponent(type)) //若旧元组包含该组件则复制
                    memcpy(
                        address + newArchetype.GetOffset(type),
                        entityInfo.components + oldArchetype.GetOffset(type),
                        size);
                else //否则通过构造函数初始化
                    newArchetype.constructors[i](address + newArchetype.GetOffset(type));
            }

            //使用新实体地址
            entityInfo.components = address;
        });
        //从旧内存中移除
        RemoveHeapItem(oldArchetype, entityInfo.indexAtHeap);
        //更新剩余的实体信息
        entityInfo.archetype = &newArchetype;
        entityInfo.indexAtHeap = newHeap.GetCount() - 1;
        //写回新实体信息
        entityInfos[entity] = entityInfo;
    }
    void World::RemoveEntity(Entity& entity)
    {
        assert(entity != Entity::Null && "目标实体为空！");
        assert(entityInfos.contains(entity) && "目标实体不存在！");

        //取出实体信息
        EntityInfo entityInfo = entityInfos.extract(entity).mapped();
        //运行析构函数
        const Archetype& archetype = *entityInfo.archetype;
        archetype.RunDestructor(entityInfo.components);
        //从内存中移除
        RemoveHeapItem(archetype, entityInfo.indexAtHeap);

        entity = Entity::Null;
    }
    bool World::HasSystem(System& system)
    {
        return systems.contains(&system);
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

        int count = 1 + (systems.contains(&system) ? systems[&system] : 0);
        systems[&system] = count;

        if (count == 1)
        {
            //首次添加，需实际注册到系统组接收事件。
            if (system.group == nullptr)
                systemGroup.AddSubSystem(system);
            else
                system.group->AddSubSystem(system);
        }
    }
    void World::AddSystem(std::initializer_list<System&> systems)
    {
        for (System& system : systems)
            AddSystem(system);
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
        assert(systems.contains(&system) && "无法移除未添加过的系统！");

        if (system.group != nullptr)
            RemoveSystem(*system.group);

        int count = systems[&system] - 1;
        if (count == 0)
            systems.erase(&system);
        else
            systems[&system] = count;

        if (count == 0)
        {
            //最后一次移除，需实际从系统组移除。
            if (system.group == nullptr)
                systemGroup.RemoveSubSystem(system);
            else
                system.group->RemoveSubSystem(system);
        }
    }

    void World::Update()
    {
        systemGroup.Update();
    }
    void World::Clear()
    {
        Update();
        assert(systems.empty() && "存在尚未回收的系统！");
    }

    void World::RemoveHeapItem(const Archetype& archetype, const int index)
    {
        Heap& heap = entities.at(&archetype);
        std::byte* element = heap.RemoveElement(index);
        //删除时末尾项会被用来替补空位，所以相关实体信息也需要更变
        Entity movedEntity = *reinterpret_cast<Entity*>(element);
        EntityInfo& movedEntityInfo = entityInfos[movedEntity];
        movedEntityInfo.components = element;
        movedEntityInfo.indexAtHeap = index;
    }
}
