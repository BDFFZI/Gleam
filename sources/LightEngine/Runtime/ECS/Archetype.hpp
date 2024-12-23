#pragma once
#include <format>
#include <array>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <cassert>
#include "Entity.hpp"
#include "Component.hpp"

namespace Light
{
    template <class TComponent, class... TComponents>
    concept ArchetypeComponentList = std::is_same_v<TComponent, Entity>;
    
    template <Component TComponent>
    struct ArchetypeComponentOperator
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

    struct Archetype
    {
        using ComponentConstructor = void(*)(std::byte*);
        using ComponentDestructor = void(*)(std::byte*);

        inline static std::vector<std::unique_ptr<Archetype>> allArchetypes = {}; //不能直接存对象，因为扩容时对象地址会变，旧指针或引用会失效

        template <Component... TComponents>
            requires ArchetypeComponentList<TComponents...>
        static Archetype& Register(const char* name)
        {
            const std::unique_ptr<Archetype>& archetype = allArchetypes.emplace_back(new Archetype());

            archetype->name = name;
            archetype->componentCount = {sizeof...(TComponents)};
            archetype->componentTypes = {typeid(TComponents)...};
            archetype->componentSizes = {sizeof(TComponents)...};
            archetype->componentOffsets.resize(sizeof...(TComponents));
            for (int i = 1; i < sizeof...(TComponents); ++i)
                archetype->componentOffsets[i] = archetype->componentOffsets[i - 1] + archetype->componentSizes[i - 1];
            archetype->constructors = {ArchetypeComponentOperator<TComponents>::Constructor...};
            archetype->destructors = {ArchetypeComponentOperator<TComponents>::Destructor...};
            archetype->size = archetype->componentOffsets[sizeof...(TComponents) - 1] + archetype->componentSizes[sizeof...(TComponents) - 1];
            for (int i = 0; i < sizeof...(TComponents); i++)
                archetype->componentOffsetsMap.insert({archetype->componentTypes[i], archetype->componentOffsets[i]});

            return *archetype;
        }

        const char* name;
        int componentCount;
        std::vector<std::type_index> componentTypes;
        std::vector<int> componentSizes;
        std::vector<int> componentOffsets;
        std::vector<ComponentConstructor> constructors;
        std::vector<ComponentDestructor> destructors;
        size_t size;
        std::unordered_map<std::type_index, int> componentOffsetsMap;

        int GetOffset(const std::type_index component) const
        {
            assert(componentOffsetsMap.contains(component) && "此原型不包含目标组件！");
            return componentOffsetsMap.at(component);
        }
        bool HasComponent(const std::type_index component) const
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
                    componentTypes[i].name(),
                    componentOffsets[i],
                    componentSizes[i]
                );
            }
            return result;
        }
        template <class... TComponents>
        bool Contains() const
        {
            std::type_index components[] = {typeid(TComponents)...};
            for (int i = 0; i < sizeof...(TComponents); ++i)
                if (componentOffsetsMap.contains(components[i]) == false)
                    return false;
            return true;
        }
        template <class... TComponents>
        std::array<int, sizeof...(TComponents)> MemoryMap() const
        {
            return {componentOffsetsMap.at(typeid(TComponents))...};
        }
    };

#define MakeArchetype(name,...)\
inline const Light::Archetype& name = Light::Archetype::Register<Light::Entity,__VA_ARGS__>(#name);
}
