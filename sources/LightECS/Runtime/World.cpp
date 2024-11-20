#include "World.h"

#include <ranges>

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
void World::RemoveEntity(const Entity entity)
{
    //取出实体信息
    EntityInfo entityInfo = entityInfos.extract(entity).mapped();
    //运行析构函数
    const Archetype& archetype = *entityInfo.archetype;
    archetype.RunDestructor(entityInfo.components);
    //从内存中移除
    RemoveHeapItem(archetype, entityInfo.indexAtHeap);
}
void World::Update()
{
    if (systemStartQueue.empty() == false)
    {
        for (const SystemInfo& systemInfo : systemStartQueue)
            systemInfo.start();
        systemUpdateQueue.insert(systemStartQueue.begin(), systemStartQueue.end());
        systemStartQueue.clear();
    }

    for (const SystemInfo& systemInfo : systemUpdateQueue)
    {
        systemInfo.update();
    }

    if (systemStopQueue.empty() == false)
    {
        for (const SystemInfo& systemInfo : std::ranges::reverse_view(systemStopQueue))
            systemInfo.stop();
        systemStopQueue.clear();
    }
}
void World::Clear()
{
    for (const SystemInfo& systemInfo : std::ranges::reverse_view(systemStopQueue))
        systemInfo.stop();
    for (const SystemInfo& systemInfo : std::ranges::reverse_view(systemUpdateQueue))
        systemInfo.stop();

    systemStartQueue.clear();
    systemStopQueue.clear();
    systemUpdateQueue.clear();
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
