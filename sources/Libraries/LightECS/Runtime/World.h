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
        const Archetype* archetype;
        std::byte* components;
        int indexAtHeap;
    };

    /**
     * 存储着所有的实体和系统
     */
    class World
    {
    public:
        static bool HasEntity(Entity entity);
        static Entity GetNextEntity();
        static EntityInfo& GetEntityInfo(Entity entity);
        static void SetEntityInfo(Entity entity, const std::optional<EntityInfo>& info);

        static Scene& CreateScene(std::string_view name);
        static Scene& GetOrCreateScene(std::string_view name);
        static Scene& GetDefaultScene();
        static std::vector<std::reference_wrapper<Scene>>& GetVisibleScenes();

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
        static void AddSystems(std::initializer_list<System*> systems);
        /**
         * @brief 移除系统
         *
         * 1. 会自动递归移除依赖的系统组
         * 2. 重复添加后需重复移除，当使用计数为0时才会真正移除系统
         * 
         * @param system 
         */
        static void RemoveSystem(System& system);
        static void RemoveSystems(std::initializer_list<System*> systems);

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
        //场景信息（实体的存储容器）
        inline static std::vector<std::unique_ptr<Scene>> allScenes = {std::make_unique<Scene>("Default")};
        inline static std::vector<std::reference_wrapper<Scene>> visibleScenes = {*allScenes[0]}; //无需显式指定就可以查询到的场景
        inline static std::reference_wrapper<Scene> defaultScene = *allScenes[0]; //操作实体时的默认的场景
        //系统信息
        inline static SystemGroup allSystems = {nullptr, System::LeftOrder, System::RightOrder};
        inline static std::unordered_map<System*, int> systemUsageCount = {};
    };
}