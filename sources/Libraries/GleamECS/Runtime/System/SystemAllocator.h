#pragma once
#include <unordered_map>

#include "System.h"
#include "SystemGroup.h"
#include "SystemInfoAllocator.h"

namespace Gleam
{
    class SystemAllocator
    {
    public:
        ~SystemAllocator();

        IOrderedSystemEvent& AddSystem(SystemInfo& systemInfo);
        template <class TSystem>
        TSystem& AddSystem()
        {
            return reinterpret_cast<TSystem&>(AddSystem(SystemInfoAllocator::CreateOrGetSystemInfo<TSystem>()));
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

        void Update();

    private:
        std::unordered_map<SystemInfo*, std::tuple<IOrderedSystemEvent*, int>> systems = {}; //当前托管的系统及其引用计数
        ISystemGroup rootSystem = {}; //顶层系统
    };
}