#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    struct Cube
    {
        float3 position;
        float3 size;

        bool IsContained(float3 point);
        float3 Closest(float3 point);
    };
}