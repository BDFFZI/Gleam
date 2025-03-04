#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

#ifdef GleamReflectionRuntime
#include "GleamReflection/Runtime/Type.h"
#endif

namespace Gleam
{
    struct Point
    {
        float3 position;
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Point, "AEC4F0BA-039D-4FF4-8B26-A76DCA7B7889")
    {
        Gleam_MakeType_AddField(position);
    }
#endif
}