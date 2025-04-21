#pragma once

#include "GleamECS/Runtime/Entity/EntityAllocator.h"
#include "GleamECS/Runtime/System/SystemAllocator.h"
#include "GleamECS/Runtime/Scene.h"
#include "View/View.h"

#define CurrentContext worldContexts.back()

namespace Gleam
{
    struct WorldContext
    {
        //实体系统数据
        EntityAllocator entityAllocator = {};
        SystemAllocator systemAllocator = {};
        //场景数据（场景不会主动回收）
        std::unordered_map<const SystemInfo*, Scene*> systemToScene = {};
        std::unordered_map<Entity, Scene*> entityToScene = {};
        Scene* activeScene = nullptr;
        std::vector<std::unique_ptr<Scene>> allScenes = {};
        //缓存的结构化更变
        std::vector<std::tuple<Entity>> removingEntities = {};
        std::vector<std::tuple<Entity, const Archetype*>> movingEntities = {};
        
        WorldContext& operator=(WorldContext&& other) noexcept
        {
            //限制重置顺序，以便正确触发回收事件
            if (this == &other)
                return *this;
            movingEntities = std::move(other.movingEntities);
            removingEntities = std::move(other.removingEntities);
            allScenes = std::move(other.allScenes);
            activeScene = other.activeScene;
            entityToScene = std::move(other.entityToScene);
            systemToScene = std::move(other.systemToScene);
            systemAllocator = std::move(other.systemAllocator);
            entityAllocator = std::move(other.entityAllocator);
            return *this;
        }
    };

    /**
     * 世界是一个预设的实体、系统集合，并提供了一些额外的帮助函数。
     */
    class World
    {
    public:
        static auto GetAllScenes()
        {
            return CurrentContext->allScenes | std::views::transform([](auto& scene) { return std::reference_wrapper(*scene); });
        }
        static WorldContext& GetCurrentContext()
        {
            return *CurrentContext;
        }

        static EntityInfoAllocator& GetEntityInfoAllocator()
        {
            return *GetEntityAllocator().GetEntityInfoAllocator();
        }
        static EntityAllocator& GetEntityAllocator()
        {
            return CurrentContext->entityAllocator;
        }
        static SystemAllocator& GetSystemAllocator()
        {
            return CurrentContext->systemAllocator;
        }
        template <class... Args>
        static View<Args...> GetView()
        {
            return Gleam::View<Args...>(GetEntityAllocator());
        }
        
        template <class... Args>
        static Entity AddSceneEntity(Args&&... args)
        {
            Entity entity = CurrentContext->entityAllocator.AddEntity(std::forward<Args>(args)...);

            if (CurrentContext->activeScene != nullptr)
                CurrentContext->activeScene->AddEntity(entity);

            return entity;
        }
        static void RemoveSceneEntity(Entity& entity);
        
        static void RemoveSceneEntityAsync(Entity& entity)
        {
            CurrentContext->removingEntities.emplace_back(entity);
            entity = Entity::Null; //避免野指针
        }
        static void MoveEntityAsync(const Entity entity, const Archetype& newArchetype)
        {
            CurrentContext->movingEntities.emplace_back(entity, &newArchetype);
        }
        static void RemoveComponentsAsync(const Entity entity, const std::initializer_list<std::reference_wrapper<const Type>> componentTypes)
        {
            Archetype& archetype = CurrentContext->entityAllocator.CreateOrGetArchetype(entity, componentTypes, {});
            MoveEntityAsync(entity, archetype);
        }

        static void AddSceneSystem(const SystemInfo& systemInfo);
        static void RemoveSceneSystem(const SystemInfo& systemInfo);
        template <class TSystem>
        static void AddSceneSystem()
        {
            AddSceneSystem(SystemInfoAllocator::CreateOrGetSystemInfo<TSystem>());
        }
        template <class TSystem>
        static void RemoveSceneSystem()
        {
            RemoveSceneSystem(SystemInfoAllocator::CreateOrGetSystemInfo<TSystem>());
        }

        /**
         * 创建一个空Scene
         * @param name
         * @param isRunning 
         * @return 
         */
        static Scene& AddScene(std::string_view name = "", bool isRunning = false);
        /**
         * 移除Scene并销毁或释放其托管的相关资源
         *
         * 释放即让场景放弃其对托管资源的所有权，原本其托管的所有System和Entity将完全交由World管理。
         * 因为除了Scene，World也有回收Entity和System的权力，当由World回收时，Scene应当释放所有权。
         * @param scene
         * @param release 
         */
        static void RemoveScene(Scene& scene, bool release = false);
        static std::optional<std::reference_wrapper<Scene>> GetScene(std::string_view name);
        static std::optional<std::reference_wrapper<Scene>> GetScene(Entity entity);
        static std::optional<std::reference_wrapper<Scene>> GetScene(const SystemInfo& system);

        static void Update();
        static void Clear();

    private:
        inline static std::unique_ptr<WorldContext> mainContext = std::make_unique<WorldContext>();
        inline static std::vector<WorldContext*> worldContexts = {mainContext.get()};

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
        static void FlushAsyncChange();
    };
}

#undef CurrentContext