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
        SystemAllocator() = default;
        SystemAllocator(SystemAllocator&& other) noexcept
        {
            systems = std::move(other.systems);
            rootSystem = std::move(other.rootSystem);
        }
        SystemAllocator& operator=(SystemAllocator&& other) noexcept
        {
            Clear();
            systems = std::move(other.systems);
            rootSystem = std::move(other.rootSystem);
            return *this;
        }
        ~SystemAllocator()
        {
            Clear();
        }

        int GetUsageCount(const SystemInfo& systemInfo) const
        {
            return std::get<1>(systems.at(&systemInfo));
        }
        const ISystemGroup& GetRootSystem() const
        {
            return rootSystem;
        }
        std::weak_ptr<IOrderedSystemEvent> GetSystemPtr(const SystemInfo& systemInfo)
        {
            return std::get<0>(systems.at(&systemInfo));
        }
        template <class TSystem>
        bool HasSystem() const
        {
            const SystemInfo& systemInfo = SystemInfoAllocator::GetSystemInfo(Type::CreateOrGet<TSystem>().GetID());
            return systems.contains(&systemInfo);
        }
        template <class TSystem>
        TSystem& GetSystem()
        {
            IOrderedSystemEvent* system = this->GetSystemPtr(SystemInfoAllocator::GetSystemInfo(Type::CreateOrGet<TSystem>().GetID())).lock().get();
            return *dynamic_cast<TSystem*>(system);
        }
        template <class TSystem>
        TSystem* TryGetSystem()
        {
            const SystemInfo& systemInfo = SystemInfoAllocator::GetSystemInfo(Type::CreateOrGet<TSystem>().GetID());
            auto it = systems.find(&systemInfo);
            if (it == systems.end())
                return nullptr;
            auto& [system,count] = it->second;
            return dynamic_cast<TSystem*>(system.get());
        }

        IOrderedSystemEvent& AddSystem(const SystemInfo& systemInfo);
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
        void RemoveSystem(const SystemInfo& systemInfo);
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
        void Clear();

    private:
        std::unordered_map<const SystemInfo*, std::tuple<std::shared_ptr<IOrderedSystemEvent>, int>> systems = {}; //当前托管的系统及其引用计数
        ISystemGroup rootSystem = {}; //顶层系统
    };
}