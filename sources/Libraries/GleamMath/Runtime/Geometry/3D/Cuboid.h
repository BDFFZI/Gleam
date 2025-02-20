#pragma once

#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

namespace Gleam
{
    struct Cuboid
    {
        static Cuboid Create(float3 center, float3 size)
        {
            float3 halfSize = size / 2;
            return {center - halfSize, center + halfSize};
        }

        float3 min;
        float3 max;

        float3 GetCenter() const
        {
            return (min + max) / 2;
        }
        float3 GetSize() const
        {
            return max - min;
        }
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Cuboid,"")
    {
        Gleam_MakeType_AddField(min);
        Gleam_MakeType_AddField(max);
    }
#endif
}