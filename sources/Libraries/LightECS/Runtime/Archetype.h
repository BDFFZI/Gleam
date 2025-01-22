#pragma once
#include <format>
#include <array>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <unordered_set>

#include "Entity.h"
#include "Component.h"

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
        template <Component... TComponents>
        static Archetype& Register(
            const char* name, const Archetype& parent,
            const std::vector<std::type_index>& queryNecessary = {},
            const std::vector<std::type_index>& queryExclusion = {})
        {
            const std::unique_ptr<Archetype>& archetype = allArchetypes.emplace_back(new Archetype());

            archetype->name = name;
            archetype->componentCount = parent.componentCount + sizeof...(TComponents);
            archetype->componentTypes = parent.componentTypes;
            archetype->componentTypes.insert(archetype->componentTypes.end(), {typeid(TComponents)...});
            archetype->componentSizes = parent.componentSizes;
            archetype->componentSizes.insert(archetype->componentSizes.end(), {sizeof(TComponents)...});
            archetype->componentOffsets.resize(archetype->componentSizes.size());
            for (size_t i = 1; i < archetype->componentOffsets.size(); ++i)
                archetype->componentOffsets[i] = archetype->componentOffsets[i - 1] + archetype->componentSizes[i - 1];

            archetype->constructors = parent.constructors;
            archetype->constructors.insert(archetype->constructors.end(), {ArchetypeComponentOperator<TComponents>::Constructor...});
            archetype->destructors = parent.destructors;
            archetype->destructors.insert(archetype->destructors.end(), {ArchetypeComponentOperator<TComponents>::Destructor...});
            archetype->size = *(archetype->componentOffsets.end() - 1) + *(archetype->componentSizes.end() - 1);
            for (size_t i = 0; i < archetype->componentTypes.size(); i++)
                archetype->componentOffsetsMap.insert({archetype->componentTypes[i], archetype->componentOffsets[i]});

            archetype->queryNecessary = queryNecessary;
            archetype->queryExclusion = queryExclusion;

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
        std::unordered_map<std::type_index, int> componentOffsetsMap; //缓存组件偏移查询信息
        std::vector<std::type_index> queryNecessary; //查询时必须包含的组件，否则不会被查询到
        std::vector<std::type_index> queryExclusion; //查询时不可包含的组件，否则不会被查询到

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
        bool IsMatched() const
        {
            std::type_index components[] = {typeid(TComponents)...};
            for (int i = 0; i < sizeof...(TComponents); ++i)
                if (componentOffsetsMap.contains(components[i]) == false)
                    return false;

            std::set<std::type_index> queryNecessarySet = {queryNecessary.begin(), queryNecessary.end()};
            for (int i = 0; i < sizeof...(TComponents); ++i)
                queryNecessarySet.erase(components[i]);
            if (!queryNecessarySet.empty())
                return false;

            std::set<std::type_index> queryExclusionSet = {queryExclusion.begin(), queryExclusion.end()};
            for (int i = 0; i < sizeof...(TComponents); ++i)
                queryExclusionSet.erase(components[i]);
            if (queryExclusionSet.size() != queryExclusion.size())
                return false;

            return true;
        }
        template <class... TComponents>
        std::array<int, sizeof...(TComponents)> MemoryMap() const
        {
            return {componentOffsetsMap.at(typeid(TComponents))...};
        }
    };

#define Light_MakeArchetype(name,...)\
inline const Light::Archetype& name = Light::Archetype::Register<Light::Entity,__VA_ARGS__>(#name);
#define Light_MakeArchetypeChild(name,parent,...)\
inline const Light::Archetype& name = Light::Archetype::Register<__VA_ARGS__>(#name,parent);
#define Light_MakeArchetypeMark(name,parent,mark)\
inline const Light::Archetype& name = Light::Archetype::Register<mark>(#name,parent,{typeid(mark)});
}