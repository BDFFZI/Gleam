#pragma once

#include "Archetype.h"
#include "Heap.h"
#include "World.h"

namespace Light
{
    class Scene
    {
    public:
        //查询实体
        Heap& GetEntityHeap(const Archetype& archetype);
        //添加实体
        std::pair<Entity, EntityInfo> AddEntity(const Archetype& archetype);
        void AddEntities(const Archetype& archetype, int count, Entity* outEntities = nullptr);
        //移除实体
        void RemoveEntity(Entity& entity);
        //移动实体
        void MoveEntity(Entity entity, const Archetype& newArchetype);
        /**
         * 更快速的简单移动实体，它假定新旧原型的数据存储布局是完全一样的，从而直接进行内存复制。
         * @param entity 
         * @param newArchetype 
         */
        void MoveEntitySimply(Entity entity, const Archetype& newArchetype);

    private:
        std::string name;
        std::unordered_map<const Archetype*, Heap> entities;
    };
}