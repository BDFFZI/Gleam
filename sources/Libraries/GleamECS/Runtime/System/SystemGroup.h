#pragma once
#include <set>

#include "System.h"

namespace Gleam
{
    struct ISystemGroup : virtual ISystemEvent
    {
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
                auto& name = system->GetName();
                Gleam_ProfilerSample(name);
#endif
                system->Update();
            }
        }

    private:
        std::set<IOrderedSystemEvent*, IOrderedSystemEventComparer> subSystems = {};
    };
}