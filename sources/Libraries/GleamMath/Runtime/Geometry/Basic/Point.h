#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    struct Point
    {
        float3 position;

        Point();
        Point(float3 position);
        Point(float x, float y, float z);
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Point, "")
    {
        Gleam_MakeType_AddField(position);
    }
#endif
}