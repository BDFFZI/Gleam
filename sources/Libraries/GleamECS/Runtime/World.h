#pragma once

#include <cassert>

#include "Heap.h"
#include "Archetype.h"
#include "System.h"

namespace Gleam
{
    /**
     * 对每个实体存储的额外信息，如地址原型信息。
     * 用于实现反向查找，从而像面对对象一样访问实体。
     */
    struct EntityInfo
    {
        const Archetype* archetype;
        int indexAtHeap;
        std::byte* components;

        EntityInfo(const Archetype& archetype, int indexAtHeap, std::byte* components);
        EntityInfo() = default;
    };

    /**
     * 世界是所有实体和系统的载体，引擎运转的核心。
     */
    class World
    {
    public:
        //世界内容
        static std::unordered_map<const Archetype*, Heap>& GetEntities();
        static SystemGroup& GetSystems();

        //查询实体
        static bool HasEntity(Entity entity);
        static Heap& GetEntityHeap(const Archetype& archetype);
        //添加实体
        static Entity AddEntity(const Archetype& archetype);
        static void AddEntities(const Archetype& archetype, int count, Entity* outEntities = nullptr);
        template <Component... TComponents>
        static Entity AddEntity(const TComponents&... components)
        {
            Archetype& archetype = Archetype::CreateOrGet({Type::CreateOrGet<TComponents>()...});
            Entity entity = AddEntity(archetype);
            SetComponents(entity, components...);
            return entity;
        }
        //移除实体
        static void RemoveEntity(Entity& entity);
        static void RemoveAllEntities();
        //移动实体
        static void MoveEntity(Entity entity, const Archetype& newArchetype);
        /**
         * 一种快速简单的实体移动，它假定新旧原型的数据存储布局是完全一样的，从而直接进行内存复制。
         * @param entity 
         * @param newArchetype 
         */
        static void MoveEntitySimply(Entity entity, const Archetype& newArchetype);

        static bool HasSystem(System& system);
        /**
         * @brief 添加系统
         *
         * 1. 会自动递归添加依赖的系统组
         * 2. 允许重复添加，会自动记录使用计数以供移除时使用
         * 
         * @param system 
         */
        static void AddSystem(System& system);
        static void AddSystems(std::initializer_list<std::reference_wrapper<System>> systems);
        /**
         * @brief 移除系统
         *
         * 1. 会自动递归移除依赖的系统组
         * 2. 重复添加后需重复移除，当使用计数为0时才会真正移除系统
         * 
         * @param system 
         */
        static void RemoveSystem(System& system);
        static void RemoveSystems(std::initializer_list<std::reference_wrapper<System>> systems);

        template <Component TComponent>
        static bool HasComponent(const Entity entity)
        {
            EntityInfo& entityInfo = entityInfos.at(entity);
            return entityInfo.archetype->HasComponent(typeid(TComponent));
        }
        template <Component TComponent>
        static std::optional<TComponent*> TryGetComponent(const Entity entity)
        {
            if (HasComponent<TComponent>(entity) == false)
                return std::nullopt;
            return &GetComponent<TComponent>(entity);
        }
        template <Component TComponent>
        static bool TryGetComponent(const Entity entity, TComponent*& component)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            if (HasComponent<TComponent>(entity) == false)
                return false;
            component = &GetComponent<TComponent>(entity);
            return true;
        }
        template <Component TComponent>
        static TComponent& GetComponent(const Entity entity)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            int offset = entityInfo.archetype->GetComponentOffset(typeid(TComponent));
            return *reinterpret_cast<TComponent*>(entityInfo.components + offset);
        }
        template <Component... TComponents>
        static void GetComponents(const Entity entity, TComponents*&... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((outComponents = reinterpret_cast<TComponents*>(entityInfo.components + archetype.GetComponentOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        static void GetComponents(const Entity entity, TComponents&... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((outComponents = *reinterpret_cast<TComponents*>(entityInfo.components + archetype.GetComponentOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        static void SetComponents(const Entity entity, const TComponents&... components)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((*reinterpret_cast<TComponents*>(entityInfo.components + archetype.GetComponentOffset(typeid(TComponents))) = components), ...);
        }

        static void Update();
        static void Clear();

    private:
        friend class WorldWindow;
        friend class EditorUI;

        //实体信息
        inline static uint32_t nextEntity = 1;
        inline static std::unordered_map<Entity, EntityInfo> entityInfos = {};

        inline static std::unordered_map<const Archetype*, Heap> entities;
        inline static SystemGroup systems = {std::nullopt}; //场景内所有系统的根系统
        //系统使用计数，实现按需自动加载和卸载系统
        inline static std::unordered_map<System*, int> systemUsageCount = {};
        //在遍历系统的时候是不能修改容器结构的，但提供的游戏事件都是遍历容器的时候运行的，所以如果用户有增删系统的需求，必须先缓存然后再执行
        inline static std::vector<System*> removingSystems = {};
        inline static std::vector<System*> addingSystems = {};

        static Entity GetNextEntity();
        static const EntityInfo& GetEntityInfo(Entity entity);
        static void SetEntityInfo(Entity entity, const std::optional<EntityInfo>& info);

    public:
        /**
         * 将缓存的添加或卸载中的System通过引用计算后，修改到实际的系统容器中
         */
        static void FlushSystemQueue();
        /**
         * 将实体从堆中移除并自动修正因此被迁移的实体信息
         * 
         * Heap容器的特点是删除时，末尾项会被用来替补空位，所以原末尾项的实体信息需要更变。该函数可以实现该功能。
         * @param heapIndex 
         * @param elementIndex
         */
        static void RemoveHeapItem(const Archetype& heapIndex, int elementIndex);
    };
}