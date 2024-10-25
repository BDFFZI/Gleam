#pragma once
#include "Vector.hpp"
#include "VectorMath.hpp"

using eulerAngles = float3;

constexpr float3 normalize(const float3& eulerAngles)
{
    return fmod(fmod(eulerAngles, 360.0f) + 360.0f, 360.0f);
}