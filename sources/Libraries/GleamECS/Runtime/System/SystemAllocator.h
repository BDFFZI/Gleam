#pragma once
#include <unordered_map>
#include <set>

#include "System.h"
#include "SystemInfoAllocator.h"


namespace Gleam
{
    struct SystemData;
    struct SystemDataComparer
    {
        bool operator()(const SystemData* left, const SystemData* right) const;
    };

    struct SystemData
    {
        const Type* type;
        int order;
        std::function<void(EntityAllocator&)> update;
        std::set<SystemData*, SystemDataComparer> subSystems;
    };


    class SystemAllocator
    {
    public:
        void AddSystem(SystemInfo& systemInfo);
        template <class TSystem>
        void AddSystem()
        {
            AddSystem(SystemInfoAllocator::CreateOrGetSystemInfo<TSystem>());
        }
        template <class... TSystem>
        void AddSystems()
        {
            (AddSystem<TSystem>(), ...);
        }
        void RemoveSystem(SystemInfo& systemInfo);
        template <class TSystem>
        void RemoveSystem()
        {
            RemoveSystem(SystemInfoAllocator::CreateOrGetSystemInfo<TSystem>());
        }
        template <class... TSystem>
        void RemoveSystems()
        {
            (RemoveSystem<TSystem>(), ...);
        }

        void Update(EntityAllocator& entityAllocator) const;

    private:
        std::unordered_map<SystemInfo*, std::tuple<SystemData, int>> systems = {}; //当前托管的系统及其引用计数
        std::set<SystemData*, SystemDataComparer> topSystems; //顶层系统
    };
}