#pragma once
#include <iostream>
#include <set>

#include "Heap.h"
#include "LightECS/Runtime/Archetype.hpp"

struct EntityInfo
{
    const Archetype* archetype;
    std::byte* components;
    int indexAtHeap;
};

struct SystemInfo
{
    using Function = void(*)();

    const type_info* type;
    int order;
    Function start;
    Function update;
    Function stop;

    bool operator<(const SystemInfo& other) const
    {
        return order < other.order;
    }
};

template <class TFunction, class... TComponents>
concept ViewIterator = requires(TFunction function, TComponents&... components) { function(components...); };

template <class TFunction, class... TComponents>
concept ViewIteratorWithEntity = requires(TFunction function, Entity& entity, TComponents&... components) { function(entity, components...); };


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

    static void AddEntity(const Archetype& archetype, Entity* outEntity = nullptr);
    static void AddEntities(const Archetype& archetype, int count, Entity* outEntities = nullptr);
    static void MoveEntity(Entity entity, const Archetype& newArchetype);
    static void RemoveEntity(Entity entity);
    template <class... TComponents, class TFunction>
        requires ViewIterator<TFunction, TComponents...>
    static void EachEntities(TFunction function)
    {
        std::cout << std: typeid(TFunction).name() << std::endl;
    }

    template <class TSystem>
    static void AddSystem()
    {
        SystemInfo systemInfo;
        systemInfo.type = &typeid(TSystem);
        systemInfo.order = TSystem::Order;
        systemInfo.start = TSystem::Start;
        systemInfo.update = TSystem::Update;
        systemInfo.stop = TSystem::Stop;
        systemInfos.insert(systemInfo);
        TSystem::Start();
    }
    template <class TSystem>
    static void RemoveSystem()
    {
        SystemInfo systemInfo;
        systemInfo.type = &typeid(TSystem);
        systemInfo.order = TSystem::Order;
        systemInfos.erase(systemInfo);
        TSystem::Stop();
    }

    static Heap& GetEntities(const Archetype& archetype)
    {
        auto iterator = entities.find(&archetype);
        if (iterator != entities.end())
            return iterator->second;

        entities.insert({&archetype, Heap(archetype.size)});
        return entities.at(&archetype);
    }
    template <class TComponent>
    static TComponent& GetComponent(const Entity entity)
    {
        EntityInfo& entityInfo = entityInfos.at(entity);
        int offset = entityInfo.archetype->GetOffset(&typeid(TComponent));
        return *reinterpret_cast<TComponent*>(entityInfo.components + offset);
    }

    static void Update()
    {
        for (const SystemInfo& systemInfo : systemInfos)
        {
            systemInfo.update();
        }
    }

private:
    inline static uint32_t nextEntity = 1;
    inline static std::unordered_map<const Archetype*, Heap> entities = {};
    inline static std::unordered_map<Entity, EntityInfo> entityInfos = {};
    inline static std::multiset<SystemInfo> systemInfos = {};

    static void RemoveHeapItem(const Archetype& archetype, int index);
};
