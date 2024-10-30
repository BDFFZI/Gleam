#pragma once
#include <format>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include "Component.h"

class ArchetypeMeta
{
public:
    virtual ~ArchetypeMeta() = default;
    virtual std::string GetName() const = 0;
    virtual size_t GetSize() const = 0;
    virtual int GetComponentCount() const = 0;
    virtual std::vector<const type_info*> GetComponentTypes() const = 0;
    virtual std::vector<int> GetComponentOffsets() const = 0;
    virtual void RunConstructor(std::byte* ptr) const = 0;
    virtual void RunDestructor(std::byte* ptr) const = 0;
    virtual std::string ToString() const = 0;
};

template <class... TComponents>
class ArchetypeMetaInstance : public ArchetypeMeta
{
public:
    consteval ArchetypeMetaInstance(const char* name): name(name)
    {
    }

private:
    using Constructor = void(*)(std::byte*);
    using Destructor = void(*)(std::byte*);

    constexpr static int componentCount = sizeof...(TComponents);
    constexpr static std::array<const type_info*, componentCount> componentTypes = {&typeid(TComponents)...};
    constexpr static std::array<int, componentCount> componentSizes = {sizeof(TComponents)...};
    constexpr static std::array<int, componentCount> componentOffsets = []
    {
        std::array<int, sizeof...(TComponents)> componentOffsets = {};
        for (int i = 1; i < sizeof...(TComponents); ++i)
            componentOffsets[i] = componentOffsets[i - 1] + componentSizes[i - 1];
        return componentOffsets;
    }();
    constexpr static std::array<Constructor, componentCount> constructors = {ComponentMeta<TComponents>::Constructor...};
    constexpr static std::array<Destructor, componentCount> destructors = {ComponentMeta<TComponents>::Destructor...};
    constexpr static size_t size = componentOffsets[componentCount - 1] + componentSizes[componentCount - 1];

    const char* name;

    std::string GetName() const override { return name; }
    size_t GetSize() const override { return size; }
    int GetComponentCount() const override { return componentCount; }
    std::vector<const type_info*> GetComponentTypes() const override { return {componentTypes.begin(), componentTypes.end()}; }
    std::vector<int> GetComponentOffsets() const override { return {componentOffsets.begin(), componentOffsets.end()}; }
    void RunConstructor(std::byte* ptr) const override
    {
        for (int i = 0; i < componentCount; ++i)
            constructors[i](ptr + componentOffsets[i]);
    }
    void RunDestructor(std::byte* ptr) const override
    {
        for (int i = 0; i < componentCount; ++i)
            destructors[i](ptr + componentOffsets[i]);
    }
    std::string ToString() const override
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
};

class Archetype
{
public:
    inline static std::vector<Archetype> allArchetypes = {};

    Archetype() = default;
    Archetype(const ArchetypeMeta* archetypeMeta)
        : archetypeMeta(archetypeMeta)
    {
        int count = archetypeMeta->GetComponentCount();
        std::vector<const type_info*> types = archetypeMeta->GetComponentTypes();
        std::vector<int> offsets = archetypeMeta->GetComponentOffsets();
        for (int i = 0; i < count; i++)
            memoryMap.insert({types[i], offsets[i]});
    }

    const ArchetypeMeta* operator->() const
    {
        return archetypeMeta;
    }

    template <class... TComponents>
    bool Contains() const
    {
        const type_info* components[] = {&typeid(TComponents)...};
        for (int i = 0; i < sizeof...(TComponents); ++i)
            if (memoryMap.contains(components[i]) == false)
                return false;
        return true;
    }
    template <class... TComponents>
    std::array<int, sizeof...(TComponents)> MemoryMap() const
    {
        return {memoryMap.at(&typeid(TComponents))...};
    }

private:
    const ArchetypeMeta* archetypeMeta;
    std::unordered_map<const type_info*, int> memoryMap;
};

template <class... TComponents>
consteval ArchetypeMetaInstance<TComponents...> CreateArchetypeMeta(const char* name)
{
    return {name};
}

template <class... TComponents, class... TParentComponents>
consteval ArchetypeMetaInstance<TParentComponents..., TComponents...> CreateArchetypeMeta(const char* name, const ArchetypeMetaInstance<TParentComponents...>& parent)
{
    return {name};
}

#define MakeArchetype(name,...)\
constexpr ArchetypeMetaInstance name = CreateArchetypeMeta<__VA_ARGS__>(#name);\
int name##ID = Archetype::allArchetypes.size();\
int name##Register = [](){Archetype::allArchetypes.emplace_back(&(name));return 0;}();

#define MakeArchetypeInherited(name,parent,...)\
constexpr ArchetypeMetaInstance name = CreateArchetypeMeta<__VA_ARGS__>(#name,parent);\
int name##ID = Archetype::allArchetypes.size();\
int name##Register = [](){Archetype::allArchetypes.emplace_back(&(name));return 0;}();
