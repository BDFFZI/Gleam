#include "Archetype.h"

#include <cassert>

namespace Light
{
    const std::vector<std::reference_wrapper<Archetype>>& Archetype::GetAllArchetypes()
    {
        return allArchetypes;
    }

    int Archetype::GetComponentCount() const
    {
        return componentCount;
    }
    const ComponentInfo& Archetype::GetComponentInfo(const int index) const
    {
        return componentInfos[index];
    }
    int Archetype::GetComponentOffset(const int index) const
    {
        return componentOffsets[index];
    }
    bool Archetype::HasComponent(const std::type_index component) const
    {
        return componentOffsetsMap.contains(component);
    }
    int Archetype::GetComponentOffset(const std::type_index component) const
    {
        assert(componentOffsetsMap.contains(component) && "此原型不包含目标组件！");
        return componentOffsetsMap.at(component);
    }
    int Archetype::GetArchetypeSize() const
    {
        return archetypeSize;
    }

    Archetype::Archetype(std::string name, const std::vector<ComponentInfo>& componentInfos)
        : name(std::move(name)),
          componentInfos(componentInfos)
    {
        componentCount = static_cast<int>(componentInfos.size());

        componentOffsets.resize(componentCount);
        for (int i = 1; i < componentCount; ++i)
            componentOffsets[i] = componentOffsets[i - 1] + componentInfos[i - 1].size;

        archetypeSize = componentOffsets[componentCount - 1] + componentInfos[componentCount - 1].size;

        for (int i = 0; i < componentCount; i++)
            componentOffsetsMap.insert({componentInfos[i].type, componentOffsets[i]});
    }
}