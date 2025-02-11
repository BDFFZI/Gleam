#include "World.h"
#include "GleamUtility/Runtime/Program.h"

namespace Gleam
{
    EntityInfo::EntityInfo(const Archetype* archetype, const int indexAtHeap, std::byte* components)
        : archetype(archetype), indexAtHeap(indexAtHeap), components(components)
    {
    }

    std::unordered_map<const Archetype*, Heap>& World::GetEntities()
    {
        return entities;
    }
    SystemGroup* World::GetSystems()
    {
        return &systems;
    }

    bool World::HasEntity(const Entity entity)
    {
        return entityInfos.contains(entity);
    }
    Heap& World::GetEntityHeap(const Archetype* archetype)
    {
        auto iterator = entities.find(archetype);
        if (iterator != entities.end())
            return iterator->second;

        entities.insert({archetype, Heap(archetype->GetArchetypeSize())});
        return entities.at(archetype);
    }

    Entity World::AddEntity(const Archetype* archetype)
    {
        //创建实体
        Entity entity = GetNextEntity();
        //申请堆内存
        Heap& heap = GetEntityHeap(archetype);
        int heapOrigin = heap.GetCount();
        std::byte* heapAddress = heap.AddElement();
        //内存赋值
        archetype->RunConstructor(heapAddress);
        *reinterpret_cast<Entity*>(heapAddress) = entity;
        //返回实体信息
        SetEntityInfo(entity, std::make_optional<EntityInfo>(archetype, heapOrigin, heapAddress));
        return entity;
    }
    void World::AddEntities(const Archetype* archetype, const int count, Entity* outEntities)
    {
        //申请堆内存
        Heap& heap = GetEntityHeap(archetype);
        int heapOrigin = heap.GetCount();
        heap.AddElements(count, [archetype,outEntities,heapOrigin](const int itemIndex, std::byte* item)
        {
            //创建实体
            Entity entity = GetNextEntity();
            //内存赋值
            archetype->RunConstructor(item);
            *reinterpret_cast<Entity*>(item) = entity;
            //返回实体信息
            SetEntityInfo(entity, std::make_optional<EntityInfo>(archetype, heapOrigin + itemIndex, item));
            if (outEntities != nullptr)outEntities[itemIndex] = entity;
        });
    }
    void World::RemoveEntity(Entity& entity)
    {
        assert(entity != Entity::Null && "实体参数不能为空！");
        assert(World::HasEntity(entity) && "实体必须存在！");

        const EntityInfo entityInfo = GetEntityInfo(entity);
        //运行析构函数
        const Archetype* archetype = entityInfo.archetype;
        archetype->RunDestructor(entityInfo.components);
        //从内存中移除
        RemoveHeapItem(archetype, entityInfo.indexAtHeap);
        //去除实体信息
        SetEntityInfo(entity, std::nullopt);
        entity = Entity::Null;
    }
    void World::RemoveAllEntities()
    {
        for (auto& [archetype, heap] : entities)
        {
            heap.ForeachElements([archetype](std::byte* address)
            {
                //去除实体信息
                Entity entity = *reinterpret_cast<Entity*>(address);
                SetEntityInfo(entity, std::nullopt);
                //运行析构函数
                archetype->RunDestructor(address);
            });
            //从内存中移除
            heap.Clear();
        }
    }
    void World::MoveEntity(const Entity entity, const Archetype* newArchetype)
    {
        assert(entity != Entity::Null && "实体参数不能为空！");
        assert(World::HasEntity(entity) && "实体必须存在！");

        //获取旧实体信息
        EntityInfo oldEntityInfo = GetEntityInfo(entity);
        const Archetype* oldArchetype = oldEntityInfo.archetype;
        //分配新内存
        Heap& newHeap = GetEntityHeap(newArchetype);
        std::byte* newAddress = newHeap.AddElement();
        //迁移内存数据
        for (int i = 0; i < newArchetype->GetComponentCount(); ++i)
        {
            //遍历每个新原形的组件

            //获取组件信息
            const ComponentInfo& componentInfo = newArchetype->GetComponentInfo(i);
            const std::type_index type = componentInfo.type;
            std::byte* componentAddress = newAddress + newArchetype->GetComponentOffset(i);
            //赋值组件内存
            if (oldArchetype->HasComponent(type)) //若旧元组包含该组件则移动数据
                componentInfo.moveConstructor(oldEntityInfo.components + oldArchetype->GetComponentOffset(type), componentAddress);
            else //否则通过构造函数初始化
                componentInfo.constructor(componentAddress);
        }
        //从旧内存中移除
        RemoveHeapItem(oldArchetype, oldEntityInfo.indexAtHeap);
        //设置新实体信息
        EntityInfo entityInfo = {newArchetype, newHeap.GetCount() - 1, newAddress};
        SetEntityInfo(entity, entityInfo);
    }
    void World::MoveEntitySimply(const Entity entity, const Archetype* newArchetype)
    {
        //获取旧实体信息
        EntityInfo oldEntityInfo = GetEntityInfo(entity);
        //分配新内存
        Heap& newHeap = GetEntityHeap(newArchetype);
        std::byte* newAddress = newHeap.AddElement();
        //将旧数据复制到新内存
        oldEntityInfo.archetype->RunMoveConstructor(oldEntityInfo.components, newAddress);
        //将旧数据从内存中移除
        RemoveHeapItem(oldEntityInfo.archetype, oldEntityInfo.indexAtHeap);
        //设置新实体信息
        EntityInfo entityInfo = {newArchetype, newHeap.GetCount() - 1, newAddress};
        SetEntityInfo(entity, entityInfo);
    }

    bool World::HasSystem(System& system)
    {
        return systemUsageCount.contains(&system);
    }
    void World::AddSystem(System& system)
    {
        if (system.GetGroup().has_value())
            AddSystem(system.GetGroup().value());
        addingSystems.push_back(&system);
    }
    void World::AddSystems(const std::initializer_list<std::reference_wrapper<System>> systems)
    {
        for (System& system : systems | UnwrapRef)
            AddSystem(system);
    }
    void World::RemoveSystem(System& system)
    {
        if (system.GetGroup().has_value())
            RemoveSystem(system.GetGroup().value());
        removingSystems.push_back(&system);
    }
    void World::RemoveSystems(const std::initializer_list<std::reference_wrapper<System>> systems)
    {
        for (System& system : systems | UnwrapRef)
            RemoveSystem(system);
    }

    void World::Update()
    {
        FlushSystemQueue(); //将更新过程中标记增删的实体或系统移入到对应的事件队列
        systems.Update(); //运行更新队列（对SubSystem会正常调用Start，故忽略显式的Start调用）
    }
    void World::Clear()
    {
        FlushSystemQueue(); //也可以不刷新SystemQueue，效果是一样的
        systems.Stop();
        RemoveAllEntities(); //移除所有实体
        systemUsageCount.clear();
        removingSystems.clear();
        addingSystems.clear();
        nextEntity = 1;
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
    void World::FlushSystemQueue()
    {
        for (auto system : addingSystems)
        {
            const int count = 1 + (systemUsageCount.contains(system) ? systemUsageCount[system] : 0);
            systemUsageCount[system] = count;

            if (count == 1)
            {
                //首次添加，需实际注册到系统组接收事件。
                system->GetGroup().value_or(systems).get().AddSubSystem(system);
                // std::cout << "Add System：" << system->GetName() << std::endl;
            }
        }
        addingSystems.clear();
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
                system->GetGroup().value_or(systems).get().RemoveSubSystem(system);
                // std::cout << "Remove System：" << system->GetName() << std::endl;
            }
        }
        removingSystems.clear();
    }
    void World::RemoveHeapItem(const Archetype* heapIndex, const int elementIndex)
    {
        //移除旧实体
        Heap& heap = GetEntityHeap(heapIndex);
        std::byte* element = heap.RemoveElement(elementIndex);
        //获取因此被移动的实体
        const Entity movedEntity = *reinterpret_cast<Entity*>(element);
        //重设实体信息
        EntityInfo movedEntityInfo = GetEntityInfo(movedEntity);
        movedEntityInfo.components = element;
        movedEntityInfo.indexAtHeap = elementIndex;
        SetEntityInfo(movedEntity, movedEntityInfo);
    }
}