#pragma once
#include "LightMath/Runtime/Vector.hpp"
#include "LightReflection/Runtime/Type.hpp"

namespace Light
{
    struct Renderer
    {
        float4 color = 1;
    };

    MakeType("", Renderer)
    {
        MakeType_AddField(color);
    }
}
