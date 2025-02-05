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
        static const std::vector<std::unique_ptr<Archetype>>& GetAllArchetypes();

        template <Component... TComponents>
            requires ArchetypeComponentList<TComponents...>
        static Archetype* Register(const char* name)
        {
            std::unique_ptr<Archetype>& archetype = allArchetypes.emplace_back(new Archetype(
                name,
                std::initializer_list<ComponentInfo>{ComponentInfoMeta<TComponents>::GetInfo()...}
            ));
            return archetype.get();
        }
        template <Component... TComponents>
        static Archetype* Register(const char* name, const Archetype* parent)
        {
            std::vector<ComponentInfo> componentInfos = parent->componentInfos;
            componentInfos.insert(componentInfos.end(), {ComponentInfoMeta<TComponents>::GetInfo()...});
            std::unique_ptr<Archetype>& archetype = allArchetypes.emplace_back(
                new Archetype(name, componentInfos)
            );
            return archetype.get();
        }

        const std::string& GetName() const;
        int GetComponentCount() const;
        const ComponentInfo& GetComponentInfo(int index) const;
        int GetComponentOffset(int index) const;

        bool HasComponent(std::type_index component) const;
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

        std::string ToString() const;

    private:
        inline static std::vector<std::unique_ptr<Archetype>> allArchetypes = {};

        std::string name;
        std::vector<ComponentInfo> componentInfos;
        int componentCount;
        std::vector<int> componentOffsets;
        int archetypeSize;
        std::unordered_map<std::type_index, int> componentOffsetsMap; //缓存组件偏移查询信息

        Archetype(std::string_view name, const std::vector<ComponentInfo>& componentInfos);
    };

#define Light_MakeArchetype(name,...)\
inline const Light::Archetype* name = Light::Archetype::Register<Light::Entity,__VA_ARGS__>(#name);
#define Light_MakeArchetypeChild(name,parent,...)\
inline const Light::Archetype* name = Light::Archetype::Register<__VA_ARGS__>(#name,parent);
}