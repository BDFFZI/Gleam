#pragma once
#include "LightMath/Runtime/Vector.h"

namespace Light
{
    struct Transform
    {
        float3 localPosition;
        float3 localRotation;
        float3 localScale = 1;
    };
}