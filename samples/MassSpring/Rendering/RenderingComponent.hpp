#pragma once
#include "LightMath/Runtime/Vector.hpp"
#include "LightEngine/Runtime/Reflection/Type.hpp"

namespace Light
{
    struct Renderer
    {
        float4 color = 1;
    };

    Light_MakeType("", Renderer)
    {
        Light_MakeType_AddField(color);
    }
}
