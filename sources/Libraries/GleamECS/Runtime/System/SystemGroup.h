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
            updatingSystems = std::move(other.updatingSystems);
        }
        ISystemGroup& operator=(ISystemGroup&& other) noexcept
        {
            updatingSystems = std::move(other.updatingSystems);
            return *this;
        }

        const auto& GetSystems() const
        {
            return updatingSystems;
        }
        void AddSystem(IOrderedSystemEvent& system)
        {
            assert(!updatingSystems.contains(&system) && "添加已存在的系统！");

            addingSystems.insert(&system);
            updatingSystems.insert(&system);
        }
        void RemoveSystem(IOrderedSystemEvent& system)
        {
            assert(updatingSystems.contains(&system) && "移除不存在的系统！");

            removingSystems.insert(&system);
            updatingSystems.erase(&system);
        }

        void Start() override
        {
            FlushAddingSystems();
        }
        void Update() override
        {
            FlushAddingSystems();
            FlushUpdatingSystems();
            FlushRemovingSystems();
        }
        void Stop() override
        {
            removingSystems.insert(updatingSystems.begin(), updatingSystems.end());
            FlushRemovingSystems();

            addingSystems.clear();
            updatingSystems.clear();
            removingSystems.clear();
        }

    private:
        std::set<IOrderedSystemEvent*, IOrderedSystemEventComparer> addingSystems = {};
        std::set<IOrderedSystemEvent*, IOrderedSystemEventComparer> updatingSystems = {};
        std::set<IOrderedSystemEvent*, IOrderedSystemEventComparer> removingSystems = {};

        void FlushAddingSystems()
        {
            for (IOrderedSystemEvent* system : addingSystems)
                system->Start();
            addingSystems.clear();
        }
        void FlushRemovingSystems()
        {
            for (auto it = removingSystems.rbegin(); it != removingSystems.rend(); ++it)
                (*it)->Stop();
            removingSystems.clear();
        }
        void FlushUpdatingSystems() const
        {
            for (IOrderedSystemEvent* system : updatingSystems)
            {
#ifdef GleamEngineEditor
                std::string_view name = typeid(*system).name();
                Gleam_ProfilerSample(name);
#endif
                system->Update();
            }
        }
    };
}