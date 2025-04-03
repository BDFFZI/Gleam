#pragma once
#include <format>
#include <array>
#include <functional>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <ranges>
#include <unordered_set>

#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    /**
     * 实体是一个数字ID，用于区分元素个体。
     * 利用实体可以在World中反向查找到实体的地址和原型信息。
     */
    enum struct Entity:uint32_t // NOLINT(performance-enum-size)
    {
        Null = 0
    };

    /**
     * 组件是用户自定义的元素内容。
     * 用户可以通过组件，利用类似鸭子类型的原理，组装和搜索元素。
     */
    template <class TComponent>
    concept Component = !std::is_pointer_v<TComponent> && !std::is_reference_v<TComponent>;

    /**
     * 原型用于存储ECS中元素的内存布局信息。
     * ECS中任何元素的内存布局，都是如下格式：|实体|组件1|组件2|...|。
     */
    class Archetype
    {
    public:
        static auto GetAllArchetypes()
        {
            auto result = allArchetypes | std::views::values | std::views::transform([](const Archetype& archetype)
            {
                return std::reference_wrapper(archetype);
            });
            return result;
        }
        static void AddCreateArchetypeEvent(const std::function<void(const Archetype&)>& event)
        {
            createArchetypeEvent.emplace_back(event);
        }
        static uuids::uuid ComputeID(const std::vector<std::reference_wrapper<const Type>>& componentTypes);

        template <Component... TComponents>
        static Archetype& Create(const std::string_view name)
        {
            Archetype archetype = {name, {&Type::CreateOrGet<TComponents>()...}};
            return allArchetypes.emplace(archetype.id, std::move(archetype)).first->second;
        }
        template <Component... TComponents>
        static Archetype& Create(const std::string_view name, const Archetype& parent)
        {
            std::vector<const Type*> types = parent.componentTypes;
            types.insert(types.end(), {&Type::CreateOrGet<TComponents>()...});

            Archetype archetype = {name, types};
            return allArchetypes.emplace(archetype.id, std::move(archetype)).first->second;
        }
        static Archetype& Create(const std::vector<std::reference_wrapper<const Type>>& componentTypes, std::string_view name = "");
        static std::optional<std::reference_wrapper<Archetype>> GetArchetype(const std::vector<std::reference_wrapper<const Type>>& componentTypes);

        bool operator==(const Archetype& other) const
        {
            return this == &other;
        }
        bool operator!=(const Archetype& other) const
        {
            return !(*this == other);
        }

        const string& GetName() const;
        uuids::uuid GetID() const;
        int GetSize() const;
        void GetComponentTypes(std::vector<std::reference_wrapper<const Type>>& result) const;

        bool HasComponent(std::type_index component) const;
        int GetComponentCount() const;
        const Type& GetComponentType(int index) const;
        int GetComponentOffset(int index) const;
        int GetComponentOffset(std::type_index component) const;

        void Construct(std::byte* address) const;
        void Destruct(std::byte* address) const;
        void MoveConstruct(std::byte* destination, std::byte* source) const;
        void Move(std::byte* destination, std::byte* source) const;
        void CopyConstruct(std::byte* destination, std::byte* source) const;
        void Copy(std::byte* destination, std::byte* source) const;

    private:
        Gleam_MakeType_Friend

        inline static std::unordered_map<uuids::uuid, Archetype> allArchetypes = {};
        inline static std::vector<std::function<void(const Archetype&)>> createArchetypeEvent;

        std::string name;
        uuids::uuid id;
        int size;
        int componentCount;
        std::vector<const Type*> componentTypes;
        std::vector<int> componentOffsets;

        std::unordered_map<std::type_index, int> componentMapping;

        Archetype(std::string_view name, std::vector<const Type*> componentTypes);
    };

    std::string to_string(const Archetype& archetype);

#define Gleam_MakeArchetype(name,...)\
inline const Gleam::Archetype& name = Gleam::Archetype::Create<__VA_ARGS__>(#name);
#define Gleam_MakeArchetypeChild(name,parent,...)\
inline const Gleam::Archetype& name = Gleam::Archetype::Create<__VA_ARGS__>(#name,parent);
}