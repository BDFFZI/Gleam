#pragma once
#include <format>
#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#include "Entity.hpp"

template <typename TComponent>
struct ComponentMeta
{
    static void Constructor(std::byte* ptr)
    {
        new(ptr) TComponent();
    }
    static void Destructor(std::byte* ptr)
    {
        reinterpret_cast<TComponent*>(ptr)->~TComponent();
    }
};

using ComponentConstructor = void(*)(std::byte*);
using ComponentDestructor = void(*)(std::byte*);

template <class TComponent, class... TComponents>
concept FirstIsEntity = std::is_same_v<TComponent, Entity>;

struct Archetype
{
    inline static std::vector<const Archetype*> allArchetypes = {};

    template <class... TComponents>
        requires FirstIsEntity<TComponents...>
    static Archetype Create(const char* name)
    {
        Archetype archetype;
        archetype.name = name;
        archetype.componentCount = {sizeof...(TComponents)};
        archetype.componentTypes = {&typeid(TComponents)...};
        archetype.componentSizes = {sizeof(TComponents)...};
        archetype.componentOffsets.resize(sizeof...(TComponents));
        for (int i = 1; i < sizeof...(TComponents); ++i)
            archetype.componentOffsets[i] = archetype.componentOffsets[i - 1] + archetype.componentSizes[i - 1];
        archetype.constructors = {ComponentMeta<TComponents>::Constructor...};
        archetype.destructors = {ComponentMeta<TComponents>::Destructor...};
        archetype.size = archetype.componentOffsets[sizeof...(TComponents) - 1] + archetype.componentSizes[sizeof...(TComponents) - 1];
        for (int i = 0; i < sizeof...(TComponents); i++)
            archetype.componentOffsetsMap.insert({archetype.componentTypes[i], archetype.componentOffsets[i]});

        allArchetypes.emplace_back(&archetype); //依赖RVO
        return archetype;
    }

    const char* name;
    int componentCount;
    std::vector<const type_info*> componentTypes;
    std::vector<int> componentSizes;
    std::vector<int> componentOffsets;
    std::vector<ComponentConstructor> constructors;
    std::vector<ComponentDestructor> destructors;
    size_t size;
    std::unordered_map<const type_info*, int> componentOffsetsMap;

    int GetOffset(const type_info* component) const
    {
        return componentOffsetsMap.at(component);
    }
    bool HasComponent(const type_info* component) const
    {
        return componentOffsetsMap.contains(component);
    }
    void RunConstructor(std::byte* ptr) const
    {
        for (int i = 0; i < componentCount; ++i)
            constructors[i](ptr + componentOffsets[i]);
    }
    void RunDestructor(std::byte* ptr) const
    {
        for (int i = 0; i < componentCount; ++i)
            destructors[i](ptr + componentOffsets[i]);
    }
    std::string ToString() const
    {
        std::string result = {name};
        for (int i = 0; i < componentCount; ++i)
        {
            result += std::format(
                "\n{:20} {:5} {:5}",
                componentTypes[i]->name(),
                componentOffsets[i],
                componentSizes[i]
            );
        }
        return result;
    }
    template <class... TComponents>
    bool Contains() const
    {
        const type_info* components[] = {&typeid(TComponents)...};
        for (int i = 0; i < sizeof...(TComponents); ++i)
            if (componentOffsetsMap.contains(components[i]) == false)
                return false;
        return true;
    }
    template <class... TComponents>
    std::array<int, sizeof...(TComponents)> MemoryMap() const
    {
        return {componentOffsetsMap.at(&typeid(TComponents))...};
    }
};

#define MakeArchetype(name,...)\
const Archetype name = Archetype::Create<Entity,__VA_ARGS__>(#name);
