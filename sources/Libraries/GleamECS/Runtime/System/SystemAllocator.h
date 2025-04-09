#pragma once
#include <unordered_map>

#include "System.h"
#include "SystemGroup.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    class SystemAllocator
    {
    public:
        const std::unordered_map<System*, int>& GetSystemUsageCount()
        {
            return systemUsageCount;
        }

        System& AddSystem(SystemInfo& systemInfo);
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
        template <class TSystem>
        std::weak_ptr<TSystem> GetSystemPtr()
        {
            auto optionalType = Type::GetType(typeid(TSystem));
            if (!optionalType.has_value())
                return {};

            auto it = systems.find(&optionalType.value().get());
            if (it == systems.end())
                return {};

            return *reinterpret_cast<std::shared_ptr<TSystem>*>(&std::get<0>(it->second));
        }
        template <class TSystem>
        TSystem& GetSystem()
        {
            return *reinterpret_cast<TSystem*>(std::get<0>(systems.at(&Type::GetType(typeid(TSystem)).value().get())).get());
        }

        void Update();
        void Clear();

    private:
        std::unordered_map<const Type*, std::tuple<std::shared_ptr<System>, int>> systems = {};
        std::unordered_map<System*, int> systemUsageCount = {}; //系统使用计数，实现按需自动加载和卸载系统
        SystemGroup rootSystem = {0}; //场景内所有系统的根系统
    };
}