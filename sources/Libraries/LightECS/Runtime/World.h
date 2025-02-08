#pragma once
#include <set>
#include <cassert>
#include <optional>

#include "Heap.h"
#include "System.h"
#include "Archetype.h"
#include "Entity.h"
#include "Scene.h"

namespace Light
{
    struct EntityInfo
    {
        Scene* scene;
        const Archetype* archetype;
        int indexAtHeap;
        std::byte* components;

        EntityInfo(Scene* scene, const Archetype* archetype, int indexAtHeap, std::byte* components);
        EntityInfo(Scene* scene, const Archetype* archetype, int indexAtHeap);
        EntityInfo() = default;
    };

    /**
     * 存储着所有的实体和系统
     */
    class World
    {
    public:
        static Entity GetNextEntity();
        static const EntityInfo& GetEntityInfo(Entity entity);
        static void SetEntityInfo(Entity entity, const std::optional<EntityInfo>& info);
        static void FlushSystemQueue();

        static Scene* GetMainScene();
        static SystemGroup* GetMainSystem();

        static bool HasEntity(Entity entity);
        static Entity AddEntity(const Archetype* archetype);
        static void RemoveEntity(Entity& entity);
        static void MoveEntity(Entity entity, const Archetype* newArchetype);
        /**
         * 一种快速简单的实体移动，它假定新旧原型的数据存储布局是完全一样的，从而直接进行内存复制。
         */
        static void MoveEntitySimply(Entity entity, const Archetype* newArchetype);

        static bool HasSystem(System& system);
        /**
         * @brief 添加系统
         *
         * 1. 会自动递归添加依赖的系统组
         * 2. 允许重复添加，会自动记录使用计数以供移除时使用
         * 
         * @param system 
         */
        static void AddSystem(System* system);
        static void AddSystems(std::initializer_list<System*> systems);
        /**
         * @brief 移除系统
         *
         * 1. 会自动递归移除依赖的系统组
         * 2. 重复添加后需重复移除，当使用计数为0时才会真正移除系统
         * 
         * @param system 
         */
        static void RemoveSystem(System* system);
        static void RemoveSystems(std::initializer_list<System*> systems);

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
        static TComponent& GetComponent(const Entity entity)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            int offset = entityInfo.archetype->GetComponentOffset(typeid(TComponent));
            return *reinterpret_cast<TComponent*>(entityInfo.components + offset);
        }
        template <Component... TComponents>
        static void GetComponents(const Entity entity, TComponents**... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((*outComponents = reinterpret_cast<TComponents*>(entityInfo.components + archetype.GetComponentOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        static void GetComponents(const Entity entity, TComponents*... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((*outComponents = *reinterpret_cast<TComponents*>(entityInfo.components + archetype.GetComponentOffset(typeid(TComponents)))), ...);
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

        static void Start();
        static void Stop();
        static void Update();

    private:
        friend class HierarchyWindow;
        //实体信息
        inline static uint32_t nextEntity = 1;
        inline static std::unordered_map<Entity, EntityInfo> entityInfos = {};
        //主场景系统
        inline static Scene mainScene = Scene{"Main"}; //所有系统默认处理的场景
        inline static SystemGroup mainSystem = {std::nullopt, System::LeftOrder, System::RightOrder}; //所有系统的根系统
        //系统使用计数，实现按需自动加载和卸载系统
        inline static std::unordered_map<System*, int> systemUsageCount = {};
        //在遍历系统的时候是不能修改容器结构的，但提供的游戏事件都是遍历容器的时候运行的，所以如果用户有增删系统的需求，必须先缓存然后再执行
        inline static std::vector<System*> removingSystems = {};
        inline static std::vector<System*> addingSystems = {};
    };
}