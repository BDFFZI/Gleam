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
        ///
        /// 系统增删为什么要延迟执行？
        /// 添加或删除系统必须先缓存然后再实际执行，因为在遍历系统的时候是不能修改容器结构的，
        /// 但提供的游戏事件都是遍历容器的时候运行的，所以为了实现在系统事件中增删系统，必须先缓存
        /// 
        /// 虽然子系统的添加删除是延迟的，但并不支持遍历时的结构化更改。
        /// 例如Start时addingSystems被占用，但用户依然可能执行AddSubSystem函数，于是就会导致遍历异常。
        /// 此处延迟触发的真实原因是因为插入系统和删除系统是无序的，但系统本身是有序的，
        /// 为了满足系统的顺序安排，只有先缓存再汇总后才可知正确的执行顺序。
        std::vector<IOrderedSystemEvent*> systemsBuffer = {};
        std::set<IOrderedSystemEvent*, IOrderedSystemEventComparer> addingSystems = {};
        std::set<IOrderedSystemEvent*, IOrderedSystemEventComparer> updatingSystems = {};
        std::set<IOrderedSystemEvent*, IOrderedSystemEventComparer> removingSystems = {};

        void FlushAddingSystems()
        {
            systemsBuffer.insert(systemsBuffer.end(), addingSystems.begin(), addingSystems.end());
            for (auto* system : systemsBuffer)
                system->Start();
            systemsBuffer.clear();
            addingSystems.clear();
        }
        void FlushRemovingSystems()
        {
            systemsBuffer.insert(systemsBuffer.end(), removingSystems.rbegin(), removingSystems.rend());
            for (auto* system : systemsBuffer)
                system->Stop();
            systemsBuffer.clear();
            removingSystems.clear();
        }
        void FlushUpdatingSystems()
        {
            systemsBuffer.insert(systemsBuffer.end(), updatingSystems.begin(), updatingSystems.end());
            for (auto* system : systemsBuffer)
            {
#ifdef GleamEngineEditor
                std::string_view name = typeid(*system).name();
                Gleam_ProfilerSample(name);
#endif
                system->Update();
            }
            systemsBuffer.clear();
        }
    };
}