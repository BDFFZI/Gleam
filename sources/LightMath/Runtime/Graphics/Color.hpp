#pragma once
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    struct Color : float4
    {
        consteval static Color Clear() { return {0, 0, 0, 0}; }
        consteval static Color White() { return {1, 1, 1}; }
        consteval static Color Black() { return {0, 0, 0}; }
        consteval static Color Red() { return {1, 0, 0}; }
        consteval static Color Green() { return {0, 1, 0}; }
        consteval static Color Blue() { return {0, 0, 1}; }
        consteval static Color Gray() { return {0.5f, 0.5f, 0.5f}; }
        consteval static Color Yellow() { return {1, 1, 0}; }
        consteval static Color Magenta() { return {1, 0, 1}; }
        consteval static Color LightRed() { return {1, 0.5f, 0.5f}; }
        consteval static Color LightGreen() { return {0.5f, 1, 0.5f}; }
        consteval static Color LightBlue() { return {0.5f, 0.5f, 1}; }
        consteval static Color LightYellow() { return {1, 1, 0.5f}; }
        consteval static Color DarkRed() { return {0.5f, 0, 0}; }
        consteval static Color DarkGreen() { return {0, 0.5f, 0}; }
        consteval static Color DarkBlue() { return {0, 0, 0.5f}; }
        consteval static Color DarkYellow() { return {0.5f, 0.5f, 0}; }

        consteval Color() = default;
        constexpr Color(float r, float g, float b, float a = 1): float4(r, g, b, a)
        {
        }
        constexpr Color(float4 color): float4(color)
        {
        }
    };
}