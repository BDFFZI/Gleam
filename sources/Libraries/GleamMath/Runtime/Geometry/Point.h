#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct Point
    {
        float3 position;
    };

    Gleam_MakeType(Point, "")
    {
        Gleam_MakeType_AddField(position);
    }
}