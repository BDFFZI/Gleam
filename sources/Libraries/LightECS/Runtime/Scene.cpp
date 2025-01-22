#include "Scene.h"

#include "World.h"

namespace Light
{
    Heap& Scene::GetEntityHeap(const Archetype& archetype)
    {
        auto iterator = entities.find(&archetype);
        if (iterator != entities.end())
            return iterator->second;

        entities.insert({&archetype, Heap(archetype.size)});
        return entities.at(&archetype);
    }

    std::pair<Entity, EntityInfo> Scene::AddEntity(const Archetype& archetype)
    {
        //创建实体
        Entity entity = World::CreateEntity();
        //申请堆内存
        Heap& heap = GetEntityHeap(archetype);
        int heapIndex = heap.GetCount();
        std::byte* heapAddress = heap.AddElement();
        //内存赋值
        archetype.RunConstructor(heapAddress);
        *reinterpret_cast<Entity*>(heapAddress) = entity;
        //返回实体信息
        return {entity, {&archetype, heapAddress, heapIndex}};
    }

    void Scene::MoveEntity(const Entity entity, const Archetype& newArchetype)
    {
        assert(entity != Entity::Null && "目标实体为空！");
        assert(entityInfos.contains(entity) && "目标实体不存在！");

        //获取旧实体信息
        EntityInfo entityInfo = entityInfos[entity];
        const Archetype& oldArchetype = *entityInfo.archetype;
        //分配新内存并复制原数据
        Heap& newHeap = GetEntityHeap(newArchetype);
        newHeap.AddElement([&entityInfo,oldArchetype,newArchetype](std::byte* address)
        {
            for (int i = 0; i < newArchetype.componentCount; ++i)
            {
                //遍历每个新原形的组件
                const std::type_index type = newArchetype.componentTypes[i];
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
    void Scene::MoveEntitySimply(const Entity entity, const Archetype& markArchetype)
    {
        //添加到新堆
        EntityInfo& entityInfo = entityInfos[entity];
        Heap& newHeap = GetEntityHeap(markArchetype);
        std::byte* newAddress = newHeap.AddElement();
        memcpy(newAddress, entityInfo.components, entityInfo.archetype->size);
        //从旧堆中移除
        RemoveHeapItem(*entityInfo.archetype, entityInfo.indexAtHeap);
        //修改实体信息
        entityInfo.components = newAddress;
        entityInfo.indexAtHeap = newHeap.GetCount() - 1;
        entityInfo.archetype = &markArchetype;
    }
    void Scene::RemoveEntity(Entity& entity)
    {
        assert(entity != Entity::Null && "目标实体为空！");
        assert(entityInfos.contains(entity) && "目标实体不存在！");

        //取出实体信息
        const EntityInfo entityInfo = entityInfos.extract(entity).mapped();
        //运行析构函数
        const Archetype& archetype = *entityInfo.archetype;
        archetype.RunDestructor(entityInfo.components);
        //从内存中移除
        RemoveHeapItem(archetype, entityInfo.indexAtHeap);

        entity = Entity::Null;
    }
    bool Scene::HasSystem(System& system)
    {
        return systemUsageCount.contains(&system);
    }

    void Scene::AddEntities(const Archetype& archetype, const int count, Entity* outEntities)
    {
        Heap& heap = GetEntityHeap(archetype);
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
}