#pragma once
#include <format>
#include <array>
#include <functional>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <ranges>

#include "Entity.h"
#include "Component.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    template <class TComponent, class... TComponents>
    concept ArchetypeComponentList = std::is_same_v<TComponent, Entity>;

    class Archetype
    {
    public:
        static auto GetAllArchetypes()
        {
            auto allArchetypes = idToArchetype | std::views::values | std::views::transform([](const Archetype* archetype)
            {
                return *archetype;
            });
            return allArchetypes;
        }

        template <Component... TComponents> requires ArchetypeComponentList<TComponents...>
        static Archetype Create(const char* name)
        {
            Archetype archetype = {name, {&Type::GetType(typeid(TComponents)).value().get()...}};
            idToArchetype.emplace(archetype.id, &archetype);
            return archetype;
        }
        template <Component... TComponents>
        static Archetype Create(const char* name, const Archetype& parent)
        {
            std::vector<const Type*> types = parent.componentTypes;
            types.insert(types.end(), {&Type::GetType(typeid(TComponents)).value().get()...});

            Archetype archetype = {name, types};
            idToArchetype.emplace(archetype.id, &archetype);
            return archetype;
        }

        const std::string& GetName() const;
        uuids::uuid GetID() const;
        int GetSize() const;

        bool HasComponent(std::type_index component) const;
        int GetComponentCount() const;
        const Type& GetComponentType(int index) const;
        int GetComponentOffset(int index) const;
        int GetComponentOffset(std::type_index component) const;
        template <class... TComponents>
        std::array<int, sizeof...(TComponents)> GetComponentOffset() const
        {
            return {GetComponentOffset(typeid(TComponents))...};
        }

        void Construct(std::byte* address) const;
        void Destruct(std::byte* address) const;
        void MoveConstruct(std::byte* source, std::byte* destination) const;
        void Move(std::byte* source, std::byte* destination) const;

    private:
        inline static std::unordered_map<uuids::uuid, const Archetype*> idToArchetype = {};

        std::string name;
        uuids::uuid id;
        int size;
        int componentCount;
        std::unordered_map<std::type_index, int> componentMapping;
        std::vector<const Type*> componentTypes;
        std::vector<int> componentOffsets;

        Archetype(std::string_view name, std::vector<const Type*> componentTypes);
        Archetype(Archetype&) = delete;
    };

    std::string to_string(const Archetype& archetype);

#define Gleam_MakeArchetype(name,...)\
inline const Gleam::Archetype name = Gleam::Archetype::Create<Gleam::Entity,__VA_ARGS__>(#name);
#define Gleam_MakeArchetypeChild(name,parent,...)\
inline const Gleam::Archetype name = Gleam::Archetype::Create<__VA_ARGS__>(#name,parent);
}