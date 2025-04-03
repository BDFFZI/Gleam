#pragma once

#include <cassert>

#include "../Heap.h"
#include "../Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
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
        EntityInfoAllocator& GetEntityInfoAllocator()
        {
            return entityInfoAllocator;
        }
        EntityAllocator& GetEntityAllocator()
        {
            return entities;
        }
        SystemGroup& GetRootSystemGroup()
        {
            return rootSystem;
        }
        void MoveEntityAllocator(EntityAllocator& entityAllocator);

        bool HasEntity(const Entity entity)
        {
            return entityInfoAllocator.HasEntity(entity);
        }

        Entity AddEntity(const Archetype& archetype)
        {
            return entities.AddEntity(archetype);
        }
        template <Component... TComponents>
        Entity AddEntity(const TComponents&... components)
        {
            Archetype& archetype = Archetype::CreateOrGet({Type::CreateOrGet<TComponents>()...});
            Entity entity = AddEntity(archetype);
            SetComponents(entity, components...);
            return entity;
        }
        void RemoveEntity(Entity& entity, bool removeFromScene = true);
        void MoveEntity(const Entity entity, const Archetype& newArchetype)
        {
            const EntityInfo& entityInfo = entityInfoAllocator.GetEntityInfo(entity);
            entityInfo.allocator->MoveEntity(entity, newArchetype);
        }
        void CopyEntity(const Entity destination, const Entity source)
        {
            entities.CopyEntity(destination, source);
        }
        Entity CloneEntity(const Entity entity, const bool addToScene = true)
        {
            return entities.CloneEntity(entity, addToScene);
        }

        void RemoveEntityAsync(Entity& entity, bool removeFromScene = true);
        void MoveEntityAsync(const Entity entity, const Archetype& newArchetype)
        {
            movingEntities.emplace_back(entity, &newArchetype);
        }

        template <class TSystem>
        void AddSystem()
        {
            if constexpr (!std::is_void_v<typename TSystem::Group>)
                AddSystem<typename TSystem::Group>();

            auto& [system,count] = systems[typeid(TSystem)];
            ++count;

            if (count == 1) //首次添加
            {
                //创建实例
                system = std::make_shared<TSystem>();
                system->world = this;
                //注册到组
                SystemGroup* group = std::is_void_v<typename TSystem::Group> ? &rootSystem : std::get<0>(systems[typeid(typename TSystem::Group)]).get();
                group->AddSubSystem(*system);
            }
        }
        template <class TSystem>
        void RemoveSystem()
        {
            if constexpr (!std::is_void_v<typename TSystem::Group>)
                RemoveSystem<typename TSystem::Group>();

            auto& [system,count] = systems.at(typeid(TSystem));
            --count;

            if (count == 0) //最终移除
            {
                //从组移除
                SystemGroup* group = std::is_void_v<typename TSystem::Group> ? &rootSystem : std::get<0>(systems[typeid(typename TSystem::Group)]).get();
                group->RemoveSubSystem(*system);
                //销毁实例
                //在Update时执行，因为要等待系统Stop事件执行完毕。
            }
        }
        template <class TSystem>
        std::weak_ptr<TSystem> FindSystem()
        {
            if (!systems.contains(typeid(TSystem)))
                return nullptr;
            return std::get<0>(systems.at(typeid(TSystem)));
        }

        bool HasSystem(System& system);
        /**
         * @brief 添加系统
         *
         * 1. 会自动递归添加依赖的系统组
         * 2. 允许重复添加，会自动记录使用计数以供移除时使用
         * 
         * @param system 
         */
        void AddSystem(System& system);
        void AddSystems(std::initializer_list<std::reference_wrapper<System>> systems);
        /**
         * @brief 移除系统
         *
         * 1. 会自动递归移除依赖的系统组
         * 2. 重复添加后需重复移除，当使用计数为0时才会真正移除系统
         * 
         * @param system
         * @param removeFromScene 
         */
        void RemoveSystem(System& system, bool removeFromScene = true);
        void RemoveSystems(std::initializer_list<std::reference_wrapper<System>> systems, bool removeFromScene = true);

        void RemoveComponentsAsync(const Entity entity, const std::initializer_list<std::reference_wrapper<const Type>> componentTypes)
        {
            Archetype& archetype = CreateOrGetArchetype(entity, componentTypes, {});
            MoveEntityAsync(entity, archetype);
        }

        void Update();
        void Clear();

    private:
        Gleam_MakeType_Friend
        friend class HierarchyWindow;
        friend class InspectorWindow;
        friend class EntityAllocator;
        friend class Scene;
        template <class T>
        friend struct FieldDataTransferrer_Transfer;
        friend void Editor_InterceptRuntimeSystem();
        friend void ExtendWorldFunction();

        //实体信息
        EntityInfoAllocator entityInfoAllocator;
        EntityAllocator entities = EntityAllocator{entityInfoAllocator};
        std::vector<std::tuple<Entity, bool>> removingEntities = {};
        std::vector<std::tuple<Entity, const Archetype*>> movingEntities = {};
        //系统信息
        std::unordered_map<std::type_index, std::tuple<std::shared_ptr<System>, int>> systems;
        std::unordered_map<System*, int> systemUsageCount = {}; //系统使用计数，实现按需自动加载和卸载系统
        SystemGroup rootSystem = {std::nullopt}; //场景内所有系统的根系统

        /**
         * 将缓存的新增、删除、移动的实体，应用修改到实际的实体容器中
         *
         * 实体增删为什么要延迟执行？
         * 1. 最佳的实体生命周期应大于系统范围，以便能全程被系统处理。例如一段场景结束时，实体需在系统之后销毁，以便系统进行回收工作，但因为系统是延迟修改，所以实体也因此需要延迟。
         * 2. 保证了销毁前后的实体视图对称，例如对实体的某种操作同时需要两个不同时间段的系统执行，如果实体在期间被删除，则其中一个系统将丢失目标。
         * 3. 实体的结构性变更无法在遍历时立即修改，若想实现该功能，则必须先缓存。
         * 4. 部分实体无法被立即销毁，例如渲染资源被异步的图形功能占用，因此必须等待相关功能（即系统的一次调用）完成后，才可处理。
         * 实际上最主要的原因是第1、4点，由于相关需求较为常用，故使用ECS实现。另外这些需求实际只要实现销毁延迟即可，但出于一致性原则，创建也采用了相同的流程。
         */
        void FlushEntityQueue();
        void FlushSystemQueue();
    };
}