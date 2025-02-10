#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct Segment
    {
        float3 positionA;
        float3 positionB;

        float GetLength() const;
    };

    Gleam_MakeType(Segment, "")
    {
        Gleam_MakeType_AddField(positionA);
        Gleam_MakeType_AddField(positionB);
    }
}