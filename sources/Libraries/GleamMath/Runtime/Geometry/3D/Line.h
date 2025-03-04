#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

#ifdef GleamReflectionRuntime
#include "GleamReflection/Runtime/Type.h"
#endif

namespace Gleam
{
    struct Line
    {
        float3 position;
        float3 direction;
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Line, "")
    {
        Gleam_MakeType_AddField(position);
        Gleam_MakeType_AddField(direction);
    }
#endif

    struct Ray
    {
        float3 position;
        float3 direction;
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Ray, "")
    {
        Gleam_MakeType_AddField(position);
        Gleam_MakeType_AddField(direction);
    }
#endif

    struct Segment
    {
        float3 pointA;
        float3 pointB;

        float GetLength() const
        {
            return distance(pointA, pointB);
        }
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Segment, "")
    {
        Gleam_MakeType_AddField(pointA);
        Gleam_MakeType_AddField(pointB);
    }
#endif
}