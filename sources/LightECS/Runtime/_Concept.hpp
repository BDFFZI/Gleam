#pragma once
#include <type_traits>

namespace Light
{
    enum struct Entity:uint32_t // NOLINT(performance-enum-size)
    {
        Null = 0
    };

    template <class TComponent>
    concept Component = !std::is_pointer_v<TComponent> && !std::is_reference_v<TComponent>;

    template <class TSystem>
    concept System = requires()
    {
        TSystem::Order;
        TSystem::Start;
        TSystem::Update;
        TSystem::Stop;
    };

    template <class TFunction, class... TComponents>
    concept ViewIterator = requires(TFunction function, TComponents&... components) { function(components...); };

    template <class TFunction, class... TComponents>
    concept ViewIteratorWithEntity = requires(TFunction function, Entity& entity, TComponents&... components) { function(entity, components...); };

    template <class TComponent, class... TComponents>
    concept ArchetypeComponentList = std::is_same_v<TComponent, Entity>;
}
