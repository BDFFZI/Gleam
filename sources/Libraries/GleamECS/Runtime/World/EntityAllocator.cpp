#include "EntityAllocator.h"

namespace Gleam
{
    std::unordered_map<const Archetype*, Heap>& EntityAllocator::GetEntityHeaps()
    {
        return entityHeaps;
    }
    Heap& EntityAllocator::GetEntityHeap(const Archetype& archetype)
    {
        auto iterator = entityHeaps.find(&archetype);
        if (iterator != entityHeaps.end())
            return iterator->second;

        entityHeaps.insert({&archetype, Heap(archetype.GetSize())});
        return entityHeaps.at(&archetype);
    }

    Entity EntityAllocator::AddEntity(const Archetype& archetype)
    {
        Entity entity;
        EntityInfo entityInfo;
        AddEntityUninitialized(archetype, entity, entityInfo);
        archetype.Construct(entityInfo.memoryAddress); //内存赋值

        return entity;
    }
    void EntityAllocator::AddEntities(const Archetype& archetype, const int count, Entity* outEntities)
    {
        //申请堆内存
        Heap& heap = GetEntityHeap(archetype);
        int heapOrigin = heap.GetCount();
        heap.AddElements(count, [this,&archetype,outEntities,heapOrigin](const int itemIndex, std::byte* item)
        {
            //创建实体
            Entity entity = entityInfoAllocator->NextEntity();
            //内存赋值
            archetype.Construct(item);
            *reinterpret_cast<Entity*>(item) = entity;
            //设置实体信息
            entityInfoAllocator->SetEntityInfo(entity, std::make_optional<EntityInfo>(archetype, *this, heapOrigin + itemIndex, item));

            if (outEntities != nullptr)
                outEntities[itemIndex] = entity;
        });
    }
    void EntityAllocator::RemoveEntity(Entity& entity)
    {
        assert(entity != Entity::Null && "实体为空！");
        assert(entityInfoAllocator->HasEntity(entity) && "实体不存在！");

        const EntityInfo entityInfo = entityInfoAllocator->GetEntityInfo(entity);
        //去除实体信息
        entityInfoAllocator->SetEntityInfo(entity, std::nullopt);
        //运行析构函数
        const Archetype* archetype = entityInfo.archetype;
        archetype->Destruct(entityInfo.memoryAddress);
        //从内存中移除
        RemoveHeapItem(*archetype, entityInfo.memoryIndex);

        entity = Entity::Null;
    }
    void EntityAllocator::MoveEntity(const Entity entity, const Archetype& newArchetype)
    {
        assert(entity != Entity::Null && "实体为空！");
        assert(entityInfoAllocator->HasEntity(entity) && "实体不存在！");

        //获取旧实体信息
        EntityInfo oldEntityInfo = entityInfoAllocator->GetEntityInfo(entity);
        const Archetype& oldArchetype = *oldEntityInfo.archetype;
        std::byte* oldAddress = oldEntityInfo.memoryAddress;
        assert(oldArchetype != newArchetype && "实体已经基于目标原型！");

        //分配新内存
        Heap& newHeap = GetEntityHeap(newArchetype);
        std::byte* newAddress = newHeap.AddElement();
        *reinterpret_cast<Entity*>(newAddress) = entity; //Entity不被归类于组件，需单独赋值
        //移动或构造新组件
        for (int i = 0; i < newArchetype.GetComponentCount(); ++i) //遍历每个新原形的组件
        {
            //获取组件信息
            const Type& componentType = newArchetype.GetComponentType(i);
            const std::type_index typeIndex = componentType.GetIndex();
            std::byte* componentAddress = newAddress + newArchetype.GetComponentOffset(i);
            //赋值组件内存
            if (oldArchetype.HasComponent(typeIndex)) //若旧元组包含该组件则移动数据
                componentType.MoveConstruct(componentAddress, oldAddress + oldArchetype.GetComponentOffset(typeIndex));
            else //否则通过构造函数初始化
                componentType.Construct(componentAddress);
        }
        //移除旧组件
        for (int i = 0; i < oldArchetype.GetComponentCount(); ++i)
        {
            const Type& componentType = oldArchetype.GetComponentType(i);
            if (newArchetype.HasComponent(componentType.GetIndex()))
                continue; //跳过在新原型依然存在的组件

            std::byte* componentAddress = oldAddress + oldArchetype.GetComponentOffset(i);
            componentType.Destruct(componentAddress);
        }

        //从堆中移除旧实体内容
        RemoveHeapItem(oldArchetype, oldEntityInfo.memoryIndex);
        //设置新实体信息
        EntityInfo entityInfo = {newArchetype, *this, newHeap.GetCount() - 1, newAddress};
        entityInfoAllocator->SetEntityInfo(entity, entityInfo);
    }
    void EntityAllocator::MoveEntitySimply(const Entity entity, const Archetype& newArchetype)
    {
        //获取旧实体信息
        EntityInfo oldEntityInfo = entityInfoAllocator->GetEntityInfo(entity);
        //分配新内存
        Heap& newHeap = GetEntityHeap(newArchetype);
        std::byte* newAddress = newHeap.AddElement();
        //将旧数据移动到新内存
        oldEntityInfo.archetype->MoveConstruct(newAddress, oldEntityInfo.memoryAddress);
        //将旧数据从内存中移除
        RemoveHeapItem(*oldEntityInfo.archetype, oldEntityInfo.memoryIndex);
        //设置新实体信息
        EntityInfo entityInfo = {newArchetype, *this, newHeap.GetCount() - 1, newAddress};
        entityInfoAllocator->SetEntityInfo(entity, entityInfo);
    }
    void EntityAllocator::CopyEntity(const Entity destination, const Entity source)
    {
        const Archetype& sourceArchetype = *entityInfoAllocator->GetEntityInfo(source).archetype;
        if (const Archetype& destinationArchetype = *entityInfoAllocator->GetEntityInfo(destination).archetype;
            destinationArchetype != sourceArchetype)
            MoveEntity(destination, sourceArchetype);

        std::byte* sourceAddress = entityInfoAllocator->GetEntityInfo(source).memoryAddress;
        std::byte* destinationAddress = entityInfoAllocator->GetEntityInfo(destination).memoryAddress;
        sourceArchetype.Copy(destinationAddress, sourceAddress);
    }
    Entity EntityAllocator::CloneEntity(const Entity source)
    {
        EntityInfo sourceInfo = entityInfoAllocator->GetEntityInfo(source);

        Entity entity;
        EntityInfo entityInfo;
        AddEntityUninitialized(*sourceInfo.archetype, entity, entityInfo);
        sourceInfo.archetype->CopyConstruct(entityInfo.memoryAddress, sourceInfo.memoryAddress);

        return entity;
    }

    void EntityAllocator::Clear()
    {
        for (auto& [archetype, heap] : entityHeaps)
        {
            heap.ForeachElements([this,archetype](std::byte* address)
            {
                Entity entity = *reinterpret_cast<Entity*>(address);
                //去除实体信息
                entityInfoAllocator->SetEntityInfo(entity, std::nullopt);
                //运行析构函数
                archetype->Destruct(address);
            });
        }
        entityHeaps.clear();
    }

    void EntityAllocator::AddEntityUninitialized(const Archetype& archetype, Entity& outEntity, EntityInfo& outEntityInfo)
    {
        //创建实体
        outEntity = entityInfoAllocator->NextEntity();
        //申请内存
        Heap& heap = GetEntityHeap(archetype);
        int heapOrigin = heap.GetCount();
        std::byte* memoryAddress = heap.AddElement();
        *reinterpret_cast<Entity*>(memoryAddress) = outEntity; //实体信息可以确定
        //设置实体信息
        outEntityInfo = {archetype, *this, heapOrigin, memoryAddress};
        entityInfoAllocator->SetEntityInfo(outEntity, outEntityInfo);
    }
    void EntityAllocator::RemoveHeapItem(const Archetype& heapIndex, const int elementIndex)
    {
        //移除旧实体
        Heap& heap = GetEntityHeap(heapIndex);
        std::byte* element = heap.RemoveElement(elementIndex);
        //调整因此被移动的实体
        if (element != nullptr)
        {
            //获取因此被移动的实体
            const Entity movedEntity = *reinterpret_cast<Entity*>(element);
            //重设实体信息
            EntityInfo movedEntityInfo = entityInfoAllocator->GetEntityInfo(movedEntity);
            assert(movedEntityInfo.memoryIndex != elementIndex && "被删除和移动的实体是同一个！");
            movedEntityInfo.memoryAddress = element;
            movedEntityInfo.memoryIndex = elementIndex;
            entityInfoAllocator->SetEntityInfo(movedEntity, movedEntityInfo);
        }
    }
}