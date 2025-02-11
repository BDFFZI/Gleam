#pragma once
#include <ranges>

namespace Gleam
{
    inline constexpr auto UnwrapRef = std::views::transform([]<typename T>(std::reference_wrapper<T> ref)-> T& { return ref.get(); });
}