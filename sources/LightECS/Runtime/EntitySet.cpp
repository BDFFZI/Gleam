#include "EntitySet.h"

EntitySet::EntitySet()
{
    for (const auto& archetype : Archetype::allArchetypes)
        entityHeaps.emplace_back(archetype->GetSize());
}
void EntitySet::AddEntity(const int archetypeID, Entity* outEntity)
{
    Heap& heap = entityHeaps[archetypeID];
    int startIndex = heap.GetCount();
    heap.AddElement([this,archetypeID,outEntity,startIndex](std::byte* item)
    {
        Archetype::allArchetypes[archetypeID]->RunConstructor(item);
        Entity entity = static_cast<Entity>(nextEntity++);
        entityInfos.insert({entity, {archetypeID, startIndex, item}});
        outEntity[0] = entity;
    });
}
void EntitySet::AddEntities(const int archetypeID, const int count, Entity* outEntities)
{
    Heap& heap = entityHeaps[archetypeID];
    int startIndex = heap.GetCount();
    heap.AddElements(count, [this,archetypeID,outEntities,startIndex](const int itemIndex, std::byte* item)
    {
        Archetype::allArchetypes[archetypeID]->RunConstructor(item);
        Entity entity = static_cast<Entity>(nextEntity++);
        entityInfos.insert({entity, {archetypeID, startIndex + itemIndex, item}});
        outEntities[itemIndex] = entity;
    });
}
void EntitySet::MoveEntity(const Entity entity, int newArchetypeID)
{
    //取出实体信息
    EntityInfo entityInfo = entityInfos[entity];
    //分配新内存并复制原数据
    const Archetype* oldArchetype = Archetype::allArchetypes[entityInfo.archetypeID];
    const Archetype* newArchetype = Archetype::allArchetypes[newArchetypeID];
    Heap& newHeap = entityHeaps[newArchetypeID];
    newHeap.AddElement([](std::byte* item)
    {
    });
    RemoveHeapItem(entityInfo.archetypeID, entityInfo.indexAtHeap);
    //写回新实体信息
    entityInfos[entity] = entityInfo;
}
void EntitySet::RemoveEntity(const Entity entity)
{
    //取出实体信息
    EntityInfo entityInfo = entityInfos.extract(entity).mapped();
    //运行析构函数
    const Archetype* archetype = Archetype::allArchetypes[entityInfo.archetypeID];
    archetype->RunDestructor(entityInfo.components);
    //从内存中移除
    RemoveHeapItem(entityInfo.archetypeID, entityInfo.indexAtHeap);
}
void EntitySet::RemoveHeapItem(const int archetypeID, const int index)
{
    Heap& heap = entityHeaps[archetypeID];
    std::byte* element = heap.RemoveElement(index);
    //删除时末尾项会被用来替补空位，所以相关实体信息也需要更变
    Entity movedEntity = *reinterpret_cast<Entity*>(element);
    EntityInfo& movedEntityInfo = entityInfos[movedEntity];
    movedEntityInfo.components = element;
    movedEntityInfo.indexAtHeap = index;
}
