#pragma once
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

    std::type_index type = typeid(void);
    int order;
    Function start;
    Function update;
    Function stop;

    bool operator<(const SystemInfo& other) const
    {
        return order < other.order;
    }
};

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

    template <class TSystem>
    static void AddSystem()
    {
        SystemInfo systemInfo;
        systemInfo.type = typeid(TSystem);
        systemInfo.order = TSystem::Order;
        systemInfo.start = TSystem::Start;
        systemInfo.update = TSystem::Update;
        systemInfo.stop = TSystem::Stop;
        systemStartQueue.insert(systemInfo);
    }
    template <class TSystem>
    static void RemoveSystem()
    {
        SystemInfo key;
        key.type = typeid(TSystem);
        SystemInfo systemInfo = systemInfos.extract(key).value();
        systemStopQueue.insert(systemInfo);
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
        int offset = entityInfo.archetype->GetOffset(typeid(TComponent));
        return *reinterpret_cast<TComponent*>(entityInfo.components + offset);
    }

    static void Update()
    {
        if (systemStopQueue.empty() == false)
        {
            for (const SystemInfo& systemInfo : systemStopQueue)
                systemInfo.stop();
            systemStopQueue.clear();
        }

        if (systemStartQueue.empty() == false)
        {
            for (const SystemInfo& systemInfo : systemStartQueue)
                systemInfo.start();
            systemInfos.insert(systemStartQueue.begin(), systemStartQueue.end());
            systemStartQueue.clear();
        }

        for (const SystemInfo& systemInfo : systemInfos)
        {
            systemInfo.update();
        }
    }

private:
    inline static uint32_t nextEntity = 1;
    inline static std::unordered_map<const Archetype*, Heap> entities = {};
    inline static std::unordered_map<Entity, EntityInfo> entityInfos = {};
    inline static std::set<SystemInfo> systemInfos = {};
    inline static std::set<SystemInfo> systemStartQueue = {};
    inline static std::set<SystemInfo> systemStopQueue = {};

    static void RemoveHeapItem(const Archetype& archetype, int index);
};
