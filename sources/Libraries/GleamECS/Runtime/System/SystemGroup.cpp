#include "SystemGroup.h"
#include <ranges>

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

            for (auto* subSystem : systemGroup->subSystems)
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
        assert(!subSystems.contains(&system) && "添加已存在的系统！");

        addingSystems.insert(&system);
    }
    void SystemGroup::RemoveSubSystem(System& system)
    {
        assert(!removingSystems.contains(&system) && "重复移除系统！");
        assert(!addingSystems.contains(&system) && "同时添加移除系统！");
        assert(subSystems.contains(&system) && "移除不存在的系统！");

        removingSystems.insert(&system);
    }

    void SystemGroup::Start()
    {
        for (System* system : addingSystems)
            system->Start();

        subSystems.insert(addingSystems.begin(), addingSystems.end());
        addingSystems.clear();
    }
    void SystemGroup::Stop()
    {
        for (System* system : std::ranges::reverse_view(subSystems))
            system->Stop();

        subSystems.clear();
        addingSystems.clear();
        removingSystems.clear();
    }

    void SystemGroup::FlushStartQueue()
    {
        for (System* system : addingSystems)
            subSystems.emplace(system);
        for (System* system : subSystems)
        {
            if (addingSystems.contains(system))
                system->Start();
            else if (SystemGroup* systemGroup = dynamic_cast<SystemGroup*>(system))
                systemGroup->FlushStartQueue();
        }
        addingSystems.clear();
    }
    void SystemGroup::FlushStopQueue()
    {
        for (System* system : std::ranges::reverse_view(subSystems))
        {
            if (removingSystems.contains(system))
                system->Stop();
            else if (SystemGroup* systemGroup = dynamic_cast<SystemGroup*>(system))
                systemGroup->FlushStopQueue();
        }
        for (System* system : removingSystems)
            subSystems.erase(system);
        removingSystems.clear();
    }
}