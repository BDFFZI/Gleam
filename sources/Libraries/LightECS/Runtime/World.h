#pragma once
#include <set>
#include <cassert>

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
        static Entity CreateEntity();
        static EntityInfo GetEntityInfo(Entity entity);
        bool HasEntity(Entity entity);


        static bool HasSystem(System& system);
        static void AddSystem(System& system);
        static void AddSystems(std::initializer_list<System*> systems);
        static void RemoveSystem(System& system);
        static void RemoveSystems(std::initializer_list<System*> systems);

        template <Component TComponent>
        static TComponent& GetComponent(const Entity entity)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            int offset = entityInfo.archetype->GetOffset(typeid(TComponent));
            return *reinterpret_cast<TComponent*>(entityInfo.components + offset);
        }
        template <Component... TComponents>
        static void GetComponents(const Entity entity, TComponents**... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((*outComponents = reinterpret_cast<TComponents*>(entityInfo.components + archetype.GetOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        static void GetComponents(const Entity entity, TComponents*... outComponents)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((*outComponents = *reinterpret_cast<TComponents*>(entityInfo.components + archetype.GetOffset(typeid(TComponents)))), ...);
        }
        template <Component... TComponents>
        static void SetComponents(const Entity entity, const TComponents&... components)
        {
            assert(entity != Entity::Null && "目标实体为空！");
            assert(entityInfos.contains(entity) && "目标实体不存在！");

            EntityInfo& entityInfo = entityInfos.at(entity);
            const Archetype& archetype = *entityInfo.archetype;
            ((*reinterpret_cast<TComponents*>(entityInfo.components + archetype.GetOffset(typeid(TComponents))) = components), ...);
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
        inline static std::vector<std::unique_ptr<Scene>> allScenes;
        inline static std::vector<Scene*> activeScenes;
        inline static Scene* defaultScenes;
        //系统信息
        inline static SystemGroup allSystems = {nullptr, System::LeftOrder, System::RightOrder};
        inline static std::unordered_map<System*, int> systemUsageCount = {};

        /**
         * 将实体从堆中移除并自动修正因此被迁移的实体信息
         * @param archetype 
         * @param index
         */
        static void RemoveHeapItem(const Archetype& archetype, int index);
    };
}