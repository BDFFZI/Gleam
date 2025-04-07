#pragma once
#include <unordered_map>
#include "Archetype.h"
#include "Heap.h"
#include "EntityInfoAllocator.h"

namespace Gleam
{
    /**
     * 实体和组件的实际内存分配器，通过获取或组装原型来得到实体的内存布局，在将其分配在@c Heap 容器中。
     */
    class EntityAllocator
    {
    public:
        explicit EntityAllocator(EntityInfoAllocator& entityInfoAllocator): entityInfoAllocator(&entityInfoAllocator)
        {
        }
        ~EntityAllocator()
        {
            Clear();
        }

        EntityInfoAllocator& GetEntityInfoAllocator() const
        {
            return *entityInfoAllocator;
        }
        std::unordered_map<const Archetype*, Heap>& GetEntityHeaps();
        Heap& GetEntityHeap(const Archetype& archetype);
        void MergeEntityAllocator(EntityAllocator& other);

        //添加实体
        Entity AddEntity(const Archetype& archetype);
        void AddEntities(const Archetype& archetype, int count, Entity* outEntities = nullptr);
        template <Component... TComponents>
        Entity AddEntity(const TComponents&... components)
        {
            Archetype& archetype = Archetype::CreateOrGet({Type::CreateOrGet<TComponents>()...});
            Entity entity = AddEntity(archetype);
            SetComponents(entity, components...);
            return entity;
        }
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

        Archetype& CreateOrGetArchetype(
            Entity entity,
            std::initializer_list<std::reference_wrapper<const Type>> removingComponents,
            std::initializer_list<std::reference_wrapper<const Type>> addingComponents) const;
        void AddComponents(const Entity entity, const std::initializer_list<std::reference_wrapper<const Type>> componentTypes)
        {
            Archetype& archetype = CreateOrGetArchetype(entity, {}, componentTypes);
            MoveEntity(entity, archetype);
        }
        void RemoveComponents(const Entity entity, const std::initializer_list<std::reference_wrapper<const Type>> componentTypes)
        {
            Archetype& archetype = CreateOrGetArchetype(entity, componentTypes, {});
            MoveEntity(entity, archetype);
        }
        template <Component TComponent>
        bool HasComponent(const Entity entity) const
        {
            const EntityInfo& entityInfo = entityInfoAllocator->GetEntityInfo(entity);
            return entityInfo.archetype->HasComponent(typeid(TComponent));
        }
        template <Component TComponent>
        std::optional<std::reference_wrapper<TComponent>> TryGetComponent(const Entity entity)
        {
            if (HasComponent<TComponent>(entity) == false)
                return std::nullopt;
            return GetComponent<TComponent>(entity);
        }
        template <Component TComponent>
        bool TryGetComponent(const Entity entity, TComponent*& outComponent)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator->HasEntity(entity) && "目标实体不存在！");

            if (HasComponent<TComponent>(entity) == false)
                return false;
            outComponent = &GetComponent<TComponent>(entity);
            return true;
        }
        template <Component TComponent>
        TComponent& GetComponent(const Entity entity)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator->HasEntity(entity) && "目标实体不存在！");

            const EntityInfo& entityInfo = entityInfoAllocator->GetEntityInfo(entity);
            int offset = entityInfo.archetype->GetComponentOffset(typeid(TComponent));
            return *reinterpret_cast<TComponent*>(entityInfo.memoryAddress + offset);
        }
        template <Component... TComponents>
        void GetComponents(const Entity entity, TComponents*&... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator->HasEntity(entity) && "目标实体不存在！");

            const EntityInfo& entityInfo = entityInfoAllocator->GetEntityInfo(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((outComponents = reinterpret_cast<TComponents*>(entityInfo.memoryAddress + archetype.GetComponentOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        void GetComponents(const Entity entity, TComponents&... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator->HasEntity(entity) && "目标实体不存在！");

            const EntityInfo& entityInfo = entityInfoAllocator->GetEntityInfo(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((outComponents = *reinterpret_cast<TComponents*>(entityInfo.memoryAddress + archetype.GetComponentOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        void SetComponents(const Entity entity, const TComponents&... components)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator->HasEntity(entity) && "目标实体不存在！");

            const EntityInfo& entityInfo = entityInfoAllocator->GetEntityInfo(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((*reinterpret_cast<TComponents*>(entityInfo.memoryAddress + archetype.GetComponentOffset(typeid(TComponents))) = components), ...);
        }

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