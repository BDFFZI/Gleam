#pragma once
#include "LightMath/Runtime/Vector.hpp"
#include "LightEngine/Runtime/Reflection/Type.hpp"

namespace Light
{
    struct Point
    {
        float2 position;
    };

    Light_MakeType("", Point)
    {
        Light_MakeType_AddField(position);
    }

    struct Line
    {
        float2 positionA;
        float2 positionB;
    };

    Light_MakeType("", Line)
    {
        Light_MakeType_AddField(positionA);
        Light_MakeType_AddField(positionB);
    }



}
