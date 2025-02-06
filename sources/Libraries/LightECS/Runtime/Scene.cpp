#include "Scene.h"

#include "World.h"

namespace Light
{
    void Scene::MoveEntity(const Entity entity, Scene* newScene)
    {
        //获取旧实体信息
        EntityInfo oldEntityInfo = World::GetEntityInfo(entity);
        Scene* oldScene = oldEntityInfo.scene;
        //分配新内存
        Heap& newHeap = newScene->GetEntityHeap(oldEntityInfo.archetype);
        std::byte* newAddress = newHeap.AddElement();
        //将旧数据复制到新内存
        oldEntityInfo.archetype->RunMoveConstructor(oldEntityInfo.components, newAddress);
        //将旧数据从内存中移除
        oldScene->RemoveHeapItem(oldEntityInfo.archetype, oldEntityInfo.indexAtHeap);
        //设置新实体信息
        EntityInfo newEntityInfo = {newScene, oldEntityInfo.archetype, newHeap.GetCount() - 1, newAddress};
        World::SetEntityInfo(entity, newEntityInfo);
    }
    void Scene::MoveAllEntities(Scene* oldScene, Scene* newScene)
    {
        for (auto& [archetype, oldHeap] : oldScene->allEntities)
        {
            Heap& newHeap = newScene->GetEntityHeap(archetype);
            oldHeap.ForeachElements([newScene,archetype,&newHeap](std::byte* oldAddress)
            {
                std::byte* newAddress = newHeap.AddElement();
                archetype->RunMoveConstructor(oldAddress, newAddress);
                EntityInfo entityInfo = {newScene, archetype, newHeap.GetCount() - 1, newAddress,};
                World::SetEntityInfo(*reinterpret_cast<Entity*>(oldAddress), entityInfo);
            });
            oldHeap.Clear();
        }
    }

    Scene::Scene(const std::string_view name)
        : name(name)
    {
    }
    const std::string& Scene::GetName()
    {
        return name;
    }
    std::unordered_map<const Archetype*, Heap>& Scene::GetAllEntities()
    {
        return allEntities;
    }

    Heap& Scene::GetEntityHeap(const Archetype* archetype)
    {
        auto iterator = allEntities.find(archetype);
        if (iterator != allEntities.end())
            return iterator->second;

        allEntities.insert({archetype, Heap(archetype->GetArchetypeSize())});
        return allEntities.at(archetype);
    }

    Entity Scene::AddEntity(const Archetype* archetype)
    {
        //创建实体
        Entity entity = World::GetNextEntity();
        //申请堆内存
        Heap& heap = GetEntityHeap(archetype);
        int heapOrigin = heap.GetCount();
        std::byte* heapAddress = heap.AddElement();
        //内存赋值
        archetype->RunConstructor(heapAddress);
        *reinterpret_cast<Entity*>(heapAddress) = entity;
        //返回实体信息
        World::SetEntityInfo(entity, std::make_optional<EntityInfo>(this, archetype, heapOrigin, heapAddress));
        return entity;
    }
    void Scene::AddEntities(const Archetype* archetype, const int count, Entity* outEntities)
    {
        //申请堆内存
        Heap& heap = GetEntityHeap(archetype);
        int heapOrigin = heap.GetCount();
        heap.AddElements(count, [this,archetype,outEntities,heapOrigin](const int itemIndex, std::byte* item)
        {
            //创建实体
            Entity entity = World::GetNextEntity();
            //内存赋值
            archetype->RunConstructor(item);
            *reinterpret_cast<Entity*>(item) = entity;
            //返回实体信息
            World::SetEntityInfo(entity, std::make_optional<EntityInfo>(this, archetype, heapOrigin + itemIndex, item));
            if (outEntities != nullptr)outEntities[itemIndex] = entity;
        });
    }

    void Scene::RemoveEntity(Entity& entity)
    {
        assert(entity != Entity::Null && "实体参数不能为空！");
        assert(World::HasEntity(entity) && "实体必须存在！");
        
        const EntityInfo entityInfo = World::GetEntityInfo(entity);
        //运行析构函数
        const Archetype* archetype = entityInfo.archetype;
        archetype->RunDestructor(entityInfo.components);
        //从内存中移除
        RemoveHeapItem(archetype, entityInfo.indexAtHeap);
        //去除实体信息
        World::SetEntityInfo(entity, std::nullopt);
        entity = Entity::Null;
    }
    void Scene::RemoveAllEntities()
    {
        for (auto& [archetype, heap] : allEntities)
        {
            heap.ForeachElements([archetype](std::byte* address)
            {
                //去除实体信息
                Entity entity = *reinterpret_cast<Entity*>(address);
                World::SetEntityInfo(entity, std::nullopt);
                //运行析构函数
                archetype->RunDestructor(address);
            });
            //从内存中移除
            heap.Clear();
        }
    }

    void Scene::MoveEntity(const Entity entity, const Archetype* newArchetype)
    {
        assert(entity != Entity::Null && "实体参数不能为空！");
        assert(World::HasEntity(entity) && "实体必须存在！");

        //获取旧实体信息
        EntityInfo oldEntityInfo = World::GetEntityInfo(entity);
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
        EntityInfo entityInfo = {this, newArchetype, newHeap.GetCount() - 1, newAddress};
        World::SetEntityInfo(entity, entityInfo);
    }
    void Scene::MoveEntitySimply(const Entity entity, const Archetype* newArchetype)
    {
        //获取旧实体信息
        EntityInfo oldEntityInfo = World::GetEntityInfo(entity);
        //分配新内存
        Heap& newHeap = GetEntityHeap(newArchetype);
        std::byte* newAddress = newHeap.AddElement();
        //将旧数据复制到新内存
        oldEntityInfo.archetype->RunMoveConstructor(oldEntityInfo.components, newAddress);
        //将旧数据从内存中移除
        RemoveHeapItem(oldEntityInfo.archetype, oldEntityInfo.indexAtHeap);
        //设置新实体信息
        EntityInfo entityInfo = {this, newArchetype, newHeap.GetCount() - 1, newAddress};
        World::SetEntityInfo(entity, entityInfo);
    }


    void Scene::RemoveHeapItem(const Archetype* heapIndex, const int elementIndex)
    {
        //移除旧实体
        Heap& heap = GetEntityHeap(heapIndex);
        std::byte* element = heap.RemoveElement(elementIndex);
        //获取因此被移动的实体
        const Entity movedEntity = *reinterpret_cast<Entity*>(element);
        //重设实体信息
        EntityInfo movedEntityInfo = World::GetEntityInfo(movedEntity);
        movedEntityInfo.components = element;
        movedEntityInfo.indexAtHeap = elementIndex;
        World::SetEntityInfo(movedEntity, movedEntityInfo);
    }
}