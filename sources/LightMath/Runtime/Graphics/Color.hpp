#pragma once
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    struct color : float4
    {
        consteval static color Clear() { return {0, 0, 0, 0}; }
        consteval static color White() { return {1, 1, 1}; }
        consteval static color Black() { return {0, 0, 0}; }
        consteval static color Red() { return {1, 0, 0}; }
        consteval static color Green() { return {0, 1, 0}; }
        consteval static color Blue() { return {0, 0, 1}; }
        consteval static color Gray() { return {0.5f, 0.5f, 0.5f}; }
        consteval static color Yellow() { return {1, 1, 0}; }
        consteval static color Magenta() { return {1, 0, 1}; }
        consteval static color LightRed() { return {1, 0.5f, 0.5f}; }
        consteval static color LightGreen() { return {0.5f, 1, 0.5f}; }
        consteval static color LightBlue() { return {0.5f, 0.5f, 1}; }
        consteval static color LightYellow() { return {1, 1, 0.5f}; }
        consteval static color DarkRed() { return {0.5f, 0, 0}; }
        consteval static color DarkGreen() { return {0, 0.5f, 0}; }
        consteval static color DarkBlue() { return {0, 0, 0.5f}; }
        consteval static color DarkYellow() { return {0.5f, 0.5f, 0}; }

        consteval color() = default;
        constexpr color(float r, float g, float b, float a = 1): float4(r, g, b, a)
        {
        }
        constexpr color(float4 color): float4(color)
        {
        }
    };
}