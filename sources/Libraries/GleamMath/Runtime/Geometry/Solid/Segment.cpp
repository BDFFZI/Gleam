#include "Segment.h"

#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

namespace Gleam
{
    Segment::Segment()
        : pointA(0), pointB(0)
    {
    }
    Segment::Segment(float3 pointA, float3 pointB)
        : pointA(pointA), pointB(pointB)
    {
    }
    float3 Segment::GetPointA() const
    {
        return pointA;
    }
    float3 Segment::GetPointB() const
    {
        return pointB;
    }
    float Segment::GetLength() const
    {
        return distance(pointA, pointB);
    }
}