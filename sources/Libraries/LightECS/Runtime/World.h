#pragma once
#include <set>
#include <cassert>

#include "Heap.h"
#include "System.h"
#include "Archetype.hpp"
#include "Entity.hpp"

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
        static Heap* GetEntityHeap(const Archetype& archetype)
        {
            auto iterator = entities.find(&archetype);
            if (iterator == entities.end())
                return nullptr;
            return &iterator->second;
        }
        static EntityInfo GetEntityInfo(Entity entity);

        static bool HasEntity(Entity entity);
        static Entity AddEntity(const Archetype& archetype);
        template <Component... TComponents>
        static Entity AddEntity(const Archetype& archetype, const TComponents&... components)
        {
            Entity entity = AddEntity(archetype);
            SetComponents(entity, components...);
            return entity;
        }
        static void AddEntities(const Archetype& archetype, int count, Entity* outEntities = nullptr);
        static void MoveEntity(Entity entity, const Archetype& newArchetype);
        static void RemoveEntity(Entity& entity);
        // /**
        //  * 为实体打上特殊标记。
        //  * 
        //  * 1. 被标记的实体将会被隐藏，查询时必须提供对应的标记才可获取到。
        //  * 2. 可以利用该功能轻松实现不同状态的实体管理，例如待唤醒/待回收/禁用中的实体。
        //  */
        // template <Component TMark>
        // static void MarkEntity(const Entity entity)
        // {
        //     EntityInfo& entityInfo = entityInfos.at(entity);
        //     entityInfo.archetype
        // }

        static bool HasSystem(System& system);
        static void AddSystem(System& system);
        static void AddSystems(std::initializer_list<System*> systems);
        static void RemoveSystem(System& system);
        static void RemoveSystems(std::initializer_list<System*> systems);

        static Heap& GetEntities(const Archetype& archetype)
        {
            auto iterator = entities.find(&archetype);
            if (iterator != entities.end())
                return iterator->second;

            entities.insert({&archetype, Heap(archetype.size)});
            return entities.at(&archetype);
        }
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
        inline static uint32_t nextEntity = 1;
        inline static std::unordered_map<const Archetype*, Heap> entities = {};
        inline static std::unordered_map<Entity, EntityInfo> entityInfos = {};
        inline static std::unordered_map<System*, int> systems = {};
        inline static SystemGroup systemGroup = {nullptr, System::LeftOrder, System::RightOrder};

        static void RemoveHeapItem(const Archetype& archetype, int index);
    };
}