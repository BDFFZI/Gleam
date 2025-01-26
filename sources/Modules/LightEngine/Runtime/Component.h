#pragma once
#include "LightMath/Runtime/Vector.h"

namespace Light
{
    struct Transform
    {
        float3 localPosition = 0;
        float3 localRotation = 0;
        float3 localScale = 1;
    };
}