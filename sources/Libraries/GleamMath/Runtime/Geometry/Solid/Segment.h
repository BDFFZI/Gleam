#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    class Segment
    {
    public:
        Segment();
        Segment(float3 pointA, float3 pointB);

        float3 GetPointA() const;
        float3 GetPointB() const;
        float GetLength() const;

    private:
#ifdef GleamReflectionRuntime
        Gleam_MakeType_Friend
#endif

        float3 pointA;
        float3 pointB;
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Segment, "")
    {
        Gleam_MakeType_AddField(pointA);
        Gleam_MakeType_AddField(pointB);
    }
#endif
}