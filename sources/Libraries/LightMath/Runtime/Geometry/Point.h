#pragma once
#include "../Vector.h"
#include "LightReflection/Runtime/Type.h"

namespace Light
{
    struct Point
    {
        float3 position;
    };

    Light_MakeType("", Point)
    {
        Light_MakeType_AddField(position);
    }
}