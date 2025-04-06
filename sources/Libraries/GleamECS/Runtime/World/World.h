#pragma once

#include <cassert>

#include "GleamECS/Runtime/Entity/EntityAllocator.h"
#include "GleamECS/Runtime/Entity/EntityInfoAllocator.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamECS/Runtime/Scene/Scene.h"

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
            return entityAllocator;
        }
        SystemGroup& GetRootSystemGroup()
        {
            return rootSystem;
        }
        auto GetAllScenes()
        {
            return allScenes | std::views::transform([](auto& scene) { return std::reference_wrapper(*scene); });
        }

        void RemoveEntityAsync(Entity& entity)
        {
            removingEntities.emplace_back(entity);
            entity = Entity::Null; //避免野指针
        }
        void MoveEntityAsync(const Entity entity, const Archetype& newArchetype)
        {
            movingEntities.emplace_back(entity, &newArchetype);
        }
        void RemoveComponentsAsync(const Entity entity, const std::initializer_list<std::reference_wrapper<const Type>> componentTypes)
        {
            Archetype& archetype = entityAllocator.CreateOrGetArchetype(entity, componentTypes, {});
            MoveEntityAsync(entity, archetype);
        }

        void AddSystem(SystemInfo& systemInfo, bool addToScene = true);
        template <class TSystem>
        void AddSystem(const bool addToScene = true)
        {
            AddSystem(System::CreateOrGetSystemInfo<TSystem>(), addToScene);
        }
        template <class... TSystem>
        void AddSystems(const bool addToScene = true)
        {
            (AddSystem<TSystem>(addToScene), ...);
        }
        void RemoveSystem(SystemInfo& systemInfo, bool removeFromScene = true);
        template <class TSystem>
        void RemoveSystem(const bool removeFromScene = true)
        {
            RemoveSystem(System::CreateOrGetSystemInfo<TSystem>(), removeFromScene);
        }
        template <class... TSystem>
        void RemoveSystems(const bool removeFromScene = true)
        {
            (RemoveSystem<TSystem>(removeFromScene), ...);
        }
        template <class TSystem>
        std::weak_ptr<TSystem> GetSystem()
        {
            if (!systems.contains(typeid(TSystem)))
                return nullptr;
            return std::get<0>(systems.at(typeid(TSystem)));
        }

        /**
         * 创建一个空Scene
         * @param name
         * @param isRunning 
         * @return 
         */
        Scene& AddScene(std::string_view name = "", bool isRunning = false);
        /**
         * 移除Scene并销毁或释放其托管的相关资源
         *
         * 释放即让场景放弃其对托管资源的所有权，原本其托管的所有System和Entity将完全交由World管理。
         * 因为除了Scene，World也有回收Entity和System的权力，当由World回收时，Scene应当释放所有权。
         * @param scene
         * @param release 
         */
        void RemoveScene(Scene& scene, bool release = false);
        std::optional<std::reference_wrapper<Scene>> GetScene(std::string_view name)
        {
            auto it = std::ranges::find_if(allScenes, [name](std::unique_ptr<Scene>& scene) { return scene->GetName() == name; });
            return it != allScenes.end() ? std::optional<std::reference_wrapper<Scene>>(**it) : std::nullopt;
        }
        std::optional<std::reference_wrapper<Scene>> GetScene(const Entity entity)
        {
            if (auto it = entityToScene.find(entity); it != entityToScene.end())
                return *it->second;
            return std::nullopt;
        }
        std::optional<std::reference_wrapper<Scene>> GetScene(SystemInfo& system)
        {
            if (auto it = systemToScene.find(&system); it != systemToScene.end())
                return *it->second;
            return std::nullopt;
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

        void AddEntityEvent(Entity entity) const;
        void RemoveEntityEvent(Entity entity);

        //实体信息
        EntityInfoAllocator entityInfoAllocator;
        EntityAllocator entityAllocator = EntityAllocator{entityInfoAllocator, [this](const Entity entity)
            {
                AddEntityEvent(entity);
            },
            [this](const Entity entity)
            {
                RemoveEntityEvent(entity);
            }};
        std::vector<std::tuple<Entity>> removingEntities = {};
        std::vector<std::tuple<Entity, const Archetype*>> movingEntities = {};
        //系统信息
        std::unordered_map<const Type*, std::tuple<std::shared_ptr<System>, int>> systems;
        std::unordered_map<System*, int> systemUsageCount = {}; //系统使用计数，实现按需自动加载和卸载系统
        SystemGroup rootSystem = {std::nullopt, 0}; //场景内所有系统的根系统
        //场景信息
        Scene* activeScene = nullptr;
        std::vector<std::unique_ptr<Scene>> allScenes = {};
        std::unordered_map<SystemInfo*, Scene*> systemToScene = {};
        std::unordered_map<Entity, Scene*> entityToScene = {};

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