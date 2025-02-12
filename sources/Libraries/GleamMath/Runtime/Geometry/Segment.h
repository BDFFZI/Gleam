#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    struct Segment
    {
        float3 positionA;
        float3 positionB;

        float GetLength() const;
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Segment, "")
    {
        Gleam_MakeType_AddField(positionA);
        Gleam_MakeType_AddField(positionB);
    }
#endif
}