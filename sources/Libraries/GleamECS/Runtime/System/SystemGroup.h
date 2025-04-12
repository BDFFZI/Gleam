#pragma once
#include <set>

#include "System.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Profiler.h"
#endif

namespace Gleam
{
    struct ISystemGroup : virtual ISystemEvent
    {
        ISystemGroup() = default;
        ISystemGroup(ISystemGroup&& other) noexcept
        {
            subSystems = std::move(other.subSystems);
        }
        ISystemGroup& operator=(ISystemGroup&& other) noexcept
        {
            subSystems = std::move(other.subSystems);
            return *this;
        }

        const auto& GetSystems() const
        {
            return subSystems;
        }
        void AddSystem(IOrderedSystemEvent& system)
        {
            assert(!subSystems.contains(&system) && "添加已存在的系统！");

            subSystems.insert(&system);
        }
        void RemoveSystem(IOrderedSystemEvent& system)
        {
            assert(subSystems.contains(&system) && "移除不存在的系统！");

            subSystems.erase(&system);
        }

        void Update() override
        {
            for (IOrderedSystemEvent* system : subSystems)
            {
#ifdef GleamEngineEditor
                std::string_view name = typeid(*system).name();
                Gleam_ProfilerSample(name);
#endif
                system->Update();
            }
        }

    private:
        std::set<IOrderedSystemEvent*, IOrderedSystemEventComparer> subSystems = {};
    };
}