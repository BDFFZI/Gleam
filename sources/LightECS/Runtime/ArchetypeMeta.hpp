#pragma once
#include "Archetype.hpp"
#include "ComponentMeta.hpp"

template <class... TComponents>
class ArchetypeMeta : public Archetype
{
public:
    consteval ArchetypeMeta(const char* name): name(name)
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
    inline static std::unordered_map<const type_info*, int> memoryMap = []()
    {
        std::unordered_map<const type_info*, int> memoryMap;
        for (int i = 0; i < componentCount; i++)
            memoryMap.insert({componentTypes[i], componentOffsets[i]});
        return memoryMap;
    }();

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
    std::unordered_map<const type_info*, int>& GetMemoryMap() const override { return memoryMap; }
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

template <class... TComponents>
consteval ArchetypeMeta<TComponents...> CreateArchetypeMeta(const char* name)
{
    return {name};
}

template <class... TComponents, class... TParentComponents>
consteval ArchetypeMeta<TParentComponents..., TComponents...> CreateArchetypeMeta(const char* name, const ArchetypeMeta<TParentComponents...>& parent)
{
    return {name};
}