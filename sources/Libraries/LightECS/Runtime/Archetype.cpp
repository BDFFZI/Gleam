#include "Archetype.h"

#include <cassert>

namespace Light
{
    const std::vector<std::unique_ptr<Archetype>>& Archetype::GetAllArchetypes()
    {
        return allArchetypes;
    }

    const std::string& Archetype::GetName() const
    {
        return name;
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
    void Archetype::RunConstructor(std::byte* ptr) const
    {
        for (int i = 0; i < componentCount; ++i)
            componentInfos[i].constructor(ptr + componentOffsets[i]);
    }
    void Archetype::RunDestructor(std::byte* ptr) const
    {
        for (int i = 0; i < componentCount; ++i)
            componentInfos[i].destructor(ptr + componentOffsets[i]);
    }
    void Archetype::RunMoveConstructor(std::byte* source, std::byte* destination) const
    {
        for (int i = 0; i < componentCount; ++i)
            componentInfos[i].moveConstructor(source + componentOffsets[i], destination + componentOffsets[i]);
    }
    std::string Archetype::ToString() const
    {
        std::string result = {name};
        for (int i = 0; i < componentCount; ++i)
        {
            result += std::format(
                "\n{:20} {:5} {:5}",
                componentInfos[i].type.name(),
                componentOffsets[i],
                componentInfos[i].size
            );
        }
        return result;
    }

    Archetype::Archetype(const std::string_view name, const std::initializer_list<ComponentInfo> componentInfos)
        : name(name),
          componentInfos(componentInfos)
    {
        componentCount = static_cast<int>(componentInfos.size());

        componentOffsets.resize(componentCount);
        for (int i = 1; i < componentCount; ++i)
            componentOffsets[i] = componentOffsets[i - 1] + this->componentInfos[i - 1].size;

        archetypeSize = componentOffsets[componentCount - 1] + this->componentInfos[componentCount - 1].size;

        for (int i = 0; i < componentCount; i++)
            componentOffsetsMap.insert({this->componentInfos[i].type, componentOffsets[i]});
    }
}