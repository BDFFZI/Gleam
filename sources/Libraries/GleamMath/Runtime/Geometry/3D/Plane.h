#pragma once
#include "Line.h"
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    struct Plane
    {
        float3 position;
        float3 normal;
    };
}