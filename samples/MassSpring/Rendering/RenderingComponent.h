#pragma once
#include "LightMath/Runtime/Vector.h"
#include "LightEngine/Runtime/Reflection/Type.h"

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
