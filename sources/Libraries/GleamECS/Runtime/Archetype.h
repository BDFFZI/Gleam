#pragma once
#include <format>
#include <array>
#include <functional>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>

#include "Entity.h"
#include "Component.h"

namespace Gleam
{
    template <class TComponent, class... TComponents>
    concept ArchetypeComponentList = std::is_same_v<TComponent, Entity>;

    class Archetype
    {
    public:
        static const std::vector<std::reference_wrapper<Archetype>>& GetAllArchetypes();
        template <Component... TComponents> requires ArchetypeComponentList<TComponents...>
        static Archetype Create(const char* name)
        {
            Archetype archetype = {
                name,
                std::initializer_list<ComponentInfo>{ComponentInfoMeta<TComponents>::GetInfo()...}
            };
            allArchetypes.emplace_back(archetype);
            return archetype;
        }
        template <Component... TComponents>
        static Archetype Create(const char* name, const Archetype& parent)
        {
            std::vector<ComponentInfo> componentInfos = parent.componentInfos;
            componentInfos.insert(componentInfos.end(), {ComponentInfoMeta<TComponents>::GetInfo()...});

            Archetype archetype = {name, componentInfos};
            allArchetypes.emplace_back(archetype);
            return archetype;
        }

        const std::string& GetName() const;
        bool HasComponent(std::type_index component) const;
        int GetComponentCount() const;
        const ComponentInfo& GetComponentInfo(int index) const;
        int GetComponentOffset(int index) const;
        int GetComponentOffset(std::type_index component) const;
        int GetArchetypeSize() const;

        void RunConstructor(std::byte* ptr) const;
        void RunDestructor(std::byte* ptr) const;
        void RunMoveConstructor(std::byte* source, std::byte* destination) const;
        template <class... TComponents>
        std::array<int, sizeof...(TComponents)> MemoryMap() const
        {
            return {componentOffsetsMap.at(typeid(TComponents))...};
        }

    private:
        inline static std::vector<std::reference_wrapper<Archetype>> allArchetypes = {};

        std::string name;
        std::vector<ComponentInfo> componentInfos;
        int componentCount;
        std::vector<int> componentOffsets;
        int archetypeSize;
        std::unordered_map<std::type_index, int> componentOffsetsMap; //缓存组件偏移查询信息

        Archetype(std::string_view name, const std::vector<ComponentInfo>& componentInfos);
    };

    std::string to_string(const Archetype& archetype);

#define Gleam_MakeArchetype(name,...)\
inline const Gleam::Archetype name = Gleam::Archetype::Create<Gleam::Entity,__VA_ARGS__>(#name);
#define Gleam_MakeArchetypeChild(name,parent,...)\
inline const Gleam::Archetype name = Gleam::Archetype::Create<__VA_ARGS__>(#name,parent);
}