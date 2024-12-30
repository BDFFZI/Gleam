#pragma once
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    struct Point : float3
    {
        Point() = default;
        Point(float x, float y, float z): float3(x, y, z)
        {
        }
        Point(const float3& v): float3(v)
        {
        }
    };
}