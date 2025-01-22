#pragma once
#include "../VectorMath.h"
#include "LightReflection/Runtime/Type.h"

namespace Light
{
    struct Segment
    {
        float3 positionA;
        float3 positionB;

        float GetLength() const;
    };

    Light_MakeType("", Segment)
    {
        Light_MakeType_AddField(positionA);
        Light_MakeType_AddField(positionB);
    }
}