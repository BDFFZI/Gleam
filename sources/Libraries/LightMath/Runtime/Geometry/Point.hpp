#pragma once
#include "../Vector.hpp"
#include "LightReflection/Runtime/Type.hpp"

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