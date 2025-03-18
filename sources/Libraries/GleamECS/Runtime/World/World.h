#pragma once

#include <cassert>

#include "../Heap.h"
#include "../Archetype.h"
#include "../System.h"
#include "EntityInfoAllocator.h"
#include "EntityAllocator.h"

namespace Gleam
{
    /**
     * 世界是所有实体和系统的载体，引擎运转的核心。
     */
    class World
    {
    public:
        //世界内容
        static EntityInfoAllocator& GetEntityInfoAllocator()
        {
            return entityInfoAllocator;
        }
        static EntityAllocator& GetEntityAllocator()
        {
            return entities;
        }
        static SystemGroup& GetRootSystemGroup()
        {
            return systems;
        }

        static bool HasEntity(Entity entity);
        static Entity AddEntity(const Archetype& archetype);
        static void RemoveEntity(Entity entity);
        static void MoveEntity(Entity entity, const Archetype& newArchetype);

        template <Component... TComponents>
        static Entity AddEntity(const TComponents&... components)
        {
            Archetype& archetype = Archetype::CreateOrGet({Type::CreateOrGet<TComponents>()...});
            Entity entity = AddEntity(archetype);
            SetComponents(entity, components...);
            return entity;
        }

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

        static void AddComponents(Entity entity, std::initializer_list<std::reference_wrapper<const Type>> componentTypes);
        static void RemoveComponents(Entity entity, std::initializer_list<std::reference_wrapper<const Type>> componentTypes);

        template <Component TComponent>
        static bool HasComponent(const Entity entity)
        {
            EntityInfo& entityInfo = entityInfoAllocator.GetEntityInfo(entity);
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
            assert(entityInfoAllocator.HasEntity(entity) && "目标实体不存在！");

            if (HasComponent<TComponent>(entity) == false)
                return false;
            component = &GetComponent<TComponent>(entity);
            return true;
        }
        template <Component TComponent>
        static TComponent& GetComponent(const Entity entity)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator.HasEntity(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfoAllocator.GetEntityInfo(entity);
            int offset = entityInfo.archetype->GetComponentOffset(typeid(TComponent));
            return *reinterpret_cast<TComponent*>(entityInfo.memoryAddress + offset);
        }
        template <Component... TComponents>
        static void GetComponents(const Entity entity, TComponents*&... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator.HasEntity(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfoAllocator.GetEntityInfo(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((outComponents = reinterpret_cast<TComponents*>(entityInfo.memoryAddress + archetype.GetComponentOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        static void GetComponents(const Entity entity, TComponents&... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator.HasEntity(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfoAllocator.GetEntityInfo(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((outComponents = *reinterpret_cast<TComponents*>(entityInfo.memoryAddress + archetype.GetComponentOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        static void SetComponents(const Entity entity, const TComponents&... components)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfoAllocator.HasEntity(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfoAllocator.GetEntityInfo(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((*reinterpret_cast<TComponents*>(entityInfo.memoryAddress + archetype.GetComponentOffset(typeid(TComponents))) = components), ...);
        }


        static void Update();
        static void Clear();

    private:
        Gleam_MakeType_Friend
        friend class HierarchyWindow;
        friend class InspectorWindow;
        friend class EntityAllocator;
        friend class Scene;
        template <class T>
        friend struct FieldDataTransferrer_Transfer;
        friend void Editor_ReplaceRuntimeSystem();
        friend void ExtendWorldFunction();

        inline static EntityInfoAllocator entityInfoAllocator;
        inline static EntityAllocator entities = EntityAllocator{entityInfoAllocator};
        /// 实体增删为什么要延迟执行？
        /// 1. 最佳的实体生命周期应大于系统范围，以便能全程被系统处理。例如一段场景结束时，实体需在系统之后销毁，以便系统进行回收工作，但因为系统是延迟修改，所以实体也因此需要延迟。
        /// 2. 保证了销毁前后的实体视图对称，例如对实体的某种操作同时需要两个不同时间段的系统执行，如果实体在期间被删除，则其中一个系统将丢失目标。
        /// 3. 实体的结构性变更无法在遍历时立即修改，若想实现该功能，则必须先缓存。
        /// 4. 部分实体无法被立即销毁，例如渲染资源被异步的图形功能占用，因此必须等待相关功能（即系统的一次调用）完成后，才可处理。
        /// 实际上最主要的原因是第1、4点，由于相关需求较为常用，故使用ECS实现。另外这些需求实际只要实现销毁延迟即可，但出于一致性原则，创建也采用了相同的流程。
        inline static EntityAllocator addingEntities = EntityAllocator{entityInfoAllocator};
        inline static std::vector<Entity> removingEntities = {};

        inline static std::unordered_map<System*, int> systemUsageCount = {}; //系统使用计数，实现按需自动加载和卸载系统
        inline static SystemGroup systems = {std::nullopt}; //场景内所有系统的根系统
        /// 添加或删除系统必须先缓存然后再实际执行，因为在遍历系统的时候是不能修改容器结构的，
        /// 但提供的游戏事件都是遍历容器的时候运行的，所以为了实现在系统事件中增删系统，必须先缓存
        inline static std::multiset<System*> removingSystems = {};
        inline static std::multiset<System*> addingSystems = {};

        /**
         * 将缓存的添加或卸载中的System通过引用计算后，修改到实际的系统容器中，
         * 此外这会清空缓冲区，从而使它们重新可用，因此后续执行系统事件时，仍能正确接收增删需求。
         */
        static void FlushSystemQueue();
        /**
         * 将缓存的新增或删除的实体，修改到实际的实体容器中
         */
        static void FlushEntityQueue();
    };
}