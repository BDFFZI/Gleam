#pragma once
#include <type_traits>

namespace Gleam
{
    template <class TComponent>
    concept Component = !std::is_pointer_v<TComponent> && !std::is_reference_v<TComponent>;
}