#pragma once
#include <unordered_map>
#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/Heap.h"
#include "EntityInfoAllocator.h"

namespace Gleam
{
    class EntityAllocator
    {
    public:
        explicit EntityAllocator(EntityInfoAllocator& entityInfoAllocator)
            : entityInfoAllocator(&entityInfoAllocator)
        {
        }

        EntityInfoAllocator& GetEntityInfoAllocator() const
        {
            return *entityInfoAllocator;
        }
        std::unordered_map<const Archetype*, Heap>& GetEntityHeaps();
        Heap& GetEntityHeap(const Archetype& archetype);

        //添加实体
        Entity AddEntity(const Archetype& archetype);
        void AddEntities(const Archetype& archetype, int count, Entity* outEntities = nullptr);
        //移除实体
        void RemoveEntity(Entity& entity);
        //移动实体
        void MoveEntity(Entity entity, const Archetype& newArchetype);
        /**
         * 一种快速简单的实体移动，它假定新旧原型的数据存储布局是完全一样的，从而直接进行内存复制。
         * @param entity 
         * @param newArchetype 
         */
        void MoveEntitySimply(Entity entity, const Archetype& newArchetype);
        //复制实体
        void CopyEntity(Entity destination, Entity source);
        Entity CloneEntity(Entity source);

        void Clear();

    private:
        EntityInfoAllocator* entityInfoAllocator = nullptr;
        std::unordered_map<const Archetype*, Heap> entityHeaps;

        void AddEntityUninitialized(const Archetype& archetype, Entity& outEntity, EntityInfo& outEntityInfo);

        /**
         * 将实体从堆内存中移除并自动修正因此被迁移的实体信息
         * 
         * Heap容器的特点是删除时，末尾项会被用来替补空位，所以原末尾项的实体信息需要更变。该函数可以实现该功能。
         * @param heapIndex 
         * @param elementIndex
         */
        void RemoveHeapItem(const Archetype& heapIndex, int elementIndex);
    };
}