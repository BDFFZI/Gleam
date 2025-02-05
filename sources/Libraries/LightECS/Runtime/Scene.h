#pragma once

#include "Archetype.h"
#include "Heap.h"

namespace Light
{
    /**
     * 一种实体容器，专门负责存储管理实体
     */
    class Scene
    {
    public:
        explicit Scene(std::string_view name = "");

        //场景属性
        const std::string& GetName();
        std::unordered_map<const Archetype*, Heap>& GetAllEntities();
        bool GetVisibility() const;
        void SetVisibility(bool visibility);

        //查询实体
        Heap& GetEntityHeap(const Archetype* archetype);
        //添加实体
        Entity AddEntity(const Archetype* archetype);
        void AddEntities(const Archetype* archetype, int count, Entity* outEntities = nullptr);
        //移除实体
        void RemoveEntity(Entity& entity);
        void RemoveAllEntities();
        //移动实体
        void MoveEntity(Entity entity, const Archetype* newArchetype);
        /**
         * 一种快速简单的实体移动，它假定新旧原型的数据存储布局是完全一样的，从而直接进行内存复制。
         * @param entity 
         * @param newArchetype 
         */
        void MoveEntitySimply(Entity entity, const Archetype* newArchetype);
        void MoveEntity(Entity entity, Scene* newScene);
        void MoveAllEntities(Scene* newScene);

    private:
        std::string name;
        bool visibility; //是否支持隐式查询
        std::unordered_map<const Archetype*, Heap> allEntities;

        /**
         * 将实体从堆中移除并自动修正因此被迁移的实体信息
         * 
         * Heap容器的特点是删除时，末尾项会被用来替补空位，所以原末尾项的实体信息需要更变。该函数可以实现该功能。
         * @param heapIndex 
         * @param elementIndex
         */
        void RemoveHeapItem(const Archetype* heapIndex, int elementIndex);
    };
}