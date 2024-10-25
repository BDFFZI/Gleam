#pragma once
#include <format>
#include <string>

template <class... TComponents>
class Archetype
{
public:
    template <class... TRest>
    consteval static bool Contains()
    {
        const std::type_info* type[] = {&typeid(TRest)...};
        for (int i = 0; i < sizeof...(TRest); ++i)
        {
            const std::type_info* target = type[i];
            bool found = false;
            for (int j = 0; j < sizeof...(TComponents); ++j)
            {
                if (componentTypes[j] == target)
                {
                    found = true;
                    break;
                }
            }
            if (found == false)
                return false;
        }

        return true;
    }

    consteval static std::array<int, sizeof...(TComponents)> InitComponentOffsets()
    {
        const int componentSizes[] = {sizeof(TComponents)...};

        std::array<int, sizeof...(TComponents)> componentOffsets = {};
        for (int i = 1; i < sizeof...(TComponents); ++i)
            componentOffsets[i] = componentOffsets[i - 1] + componentSizes[i - 1];
        return componentOffsets;
    }

    constexpr static int componentCount = sizeof...(TComponents);
    constexpr static const type_info* componentTypes[sizeof...(TComponents)] = {&typeid(TComponents)...};
    constexpr static std::array<int, componentCount> componentSizes = {};
    constexpr static std::array<int, componentCount> componentOffsets = InitComponentOffsets();
};

template <class... TComponents>
std::string to_string(const Archetype<TComponents...>& archetype)
{
    std::string result = {};
    for (int i = 0; i < archetype.componentCount; ++i)
    {
        result += std::format(
            "{:20} {:5} {:5}\n",
            archetype.componentTypes[i]->name(),
            archetype.componentOffsets[i],
            archetype.componentSizes[i]
        );
    }
    return result;
}


template <class... TComponents>
consteval Archetype<TComponents...> RegisterArchetype()
{
    return {};
}

template <class... TComponents, class... TParentComponents>
consteval Archetype<TParentComponents..., TComponents...> RegisterArchetype(const Archetype<TParentComponents...>& parent)
{
    return {};
}
