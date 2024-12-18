#pragma once
#include "LightMath/Runtime/Vector.hpp"
#include "LightReflection/Runtime/Type.hpp"

namespace Light
{
    struct Point
    {
        float2 position;
    };

    MakeType("", Point)
    {
        MakeType_AddField(position);
    }

    struct Line
    {
        float2 positionA;
        float2 positionB;
    };

    MakeType("", Line)
    {
        MakeType_AddField(positionA);
        MakeType_AddField(positionB);
    }



}
