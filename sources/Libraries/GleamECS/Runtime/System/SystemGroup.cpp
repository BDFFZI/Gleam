#include "SystemGroup.h"

namespace Gleam
{
    SystemGroup::SystemGroup(const std::optional<std::reference_wrapper<SystemGroup>>& group, const int minOrder, const int maxOrder, const std::string_view& name)
        : System(group, minOrder, maxOrder, name)
    {
    }
    SystemGroup::SystemGroup(System& system, const OrderRelation orderRelation, const std::string_view& name)
        : System(system, orderRelation, name)
    {
    }
    SystemGroup::SystemGroup(SystemGroup& group): System(group)
    {
    }

    std::vector<std::reference_wrapper<System>> SystemGroup::GetSubSystems()
    {
        std::vector<std::reference_wrapper<System>> outSubSystems;

        std::vector subSystemGroups = {this};
        while (!subSystemGroups.empty())
        {
            SystemGroup* systemGroup = subSystemGroups.back();
            subSystemGroups.pop_back();

            for (auto* subSystem : systemGroup->addingSystems)
            {
                outSubSystems.emplace_back(*subSystem);
                if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                    subSystemGroups.push_back(subSystemGroup);
            }

            for (auto* subSystem : systemGroup->updatingSystems)
            {
                outSubSystems.emplace_back(*subSystem);
                if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                    subSystemGroups.push_back(subSystemGroup);
            }
        }
        return outSubSystems;
    }
    void SystemGroup::AddSubSystem(System& system)
    {
        assert(!addingSystems.contains(&system) && "重复添加系统！");
        assert(!removingSystems.contains(&system) && "同时添加移除系统！");
        assert(!updatingSystems.contains(&system) && "添加已存在的系统！");

        addingSystems.insert(&system);
        updatingSystems.insert(&system);
    }
    void SystemGroup::RemoveSubSystem(System& system)
    {
        assert(!removingSystems.contains(&system) && "重复移除系统！");
        assert(updatingSystems.contains(&system) && "移除不存在的系统！");

        //允许撤回添加的系统，以支持编辑器下的运行时系统替换
        if (addingSystems.contains(&system))
        {
            addingSystems.erase(&system);
            updatingSystems.erase(&system);
            return;
        }

        removingSystems.insert(&system);
        updatingSystems.erase(&system);
    }

    void SystemGroup::Start()
    {
        FlushAddingSystems();
    }
    void SystemGroup::Stop()
    {
        removingSystems.insert(updatingSystems.begin(), updatingSystems.end());
        FlushRemovingSystems();

        addingSystems.clear();
        updatingSystems.clear();
        removingSystems.clear();
    }
    void SystemGroup::FlushAddingSystems()
    {
        systemsBuffer.insert(systemsBuffer.end(), addingSystems.begin(), addingSystems.end());
        for (System* system : systemsBuffer)
            system->Start();
        systemsBuffer.clear();
        addingSystems.clear();
    }
    void SystemGroup::FlushRemovingSystems()
    {
        systemsBuffer.insert(systemsBuffer.end(), removingSystems.rbegin(), removingSystems.rend());
        for (System* system : systemsBuffer)
            system->Stop();
        systemsBuffer.clear();
        removingSystems.clear();
    }
}