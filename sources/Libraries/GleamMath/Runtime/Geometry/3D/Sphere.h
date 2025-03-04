#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

#ifdef GleamReflectionRuntime
#include "GleamReflection/Runtime/Type.h"
#endif

namespace Gleam
{
    struct Sphere
    {
        float3 center;
        float radius;
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Sphere,"E81919B5-E944-4552-B874-453487FE3D62")
    {
        Gleam_MakeType_AddField(center);
        Gleam_MakeType_AddField(radius);
    }
#endif
}