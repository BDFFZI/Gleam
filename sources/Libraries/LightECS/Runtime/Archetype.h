#pragma once
#include <format>
#include <array>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Entity.h"
#include "Component.h"

namespace Light
{
    template <class TComponent, class... TComponents>
    concept ArchetypeComponentList = std::is_same_v<TComponent, Entity>;

    class Archetype
    {
    public:
        static const std::vector<std::reference_wrapper<Archetype>>& GetAllArchetypes();

        template <Component... TComponents>
            requires ArchetypeComponentList<TComponents...>
        static Archetype Register(const char* name)
        {
            Archetype archetype = {
                name,
                {ComponentInfoMeta<TComponents>::info...}
            };
            allArchetypes.emplace_back(archetype);
            return archetype;
        }

        int GetComponentCount() const;
        const ComponentInfo& GetComponentInfo(int index) const;
        int GetComponentOffset(int index) const;

        bool HasComponent(std::type_index component) const;
        int GetComponentOffset(std::type_index component) const;
        int GetArchetypeSize() const;
        void RunConstructor(std::byte* ptr) const
        {
            for (int i = 0; i < componentCount; ++i)
                componentInfos[i].constructor(ptr + componentOffsets[i]);
        }
        void RunDestructor(std::byte* ptr) const
        {
            for (int i = 0; i < componentCount; ++i)
                componentInfos[i].destructor(ptr + componentOffsets[i]);
        }

        template <class... TComponents>
        bool IsMatched() const
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

        std::string ToString() const
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

    private:
        inline static std::vector<std::reference_wrapper<Archetype>> allArchetypes = {};

        std::string name;
        std::vector<ComponentInfo> componentInfos;
        int componentCount;
        std::vector<int> componentOffsets;
        int archetypeSize;
        std::unordered_map<std::type_index, int> componentOffsetsMap; //缓存组件偏移查询信息

        Archetype(std::string name, const std::vector<ComponentInfo>& componentInfos);
    };

#define Light_MakeArchetype(name,...)\
inline const Light::Archetype name = Light::Archetype::Register<Light::Entity,__VA_ARGS__>(#name);
}