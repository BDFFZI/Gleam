#include "Cuboid.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

namespace Gleam
{
    Cuboid Cuboid::Create(float3 center, float3 size)
    {
        float3 halfSize = size / 2;
        return {center - halfSize, center + halfSize};
    }
    Cuboid::Cuboid()
        : Cuboid(0, 0)
    {
    }
    Cuboid::Cuboid(const float3 min, const float3 max)
        : min(min), max(max)
    {
    }
    float3 Cuboid::GetMin() const
    {
        return min;
    }
    float3 Cuboid::GetMax() const
    {
        return max;
    }
    float3 Cuboid::GetCenter() const
    {
        return (min + max) / 2;
    }
    float3 Cuboid::GetSize() const
    {
        return max - min;
    }

    bool Cuboid::Contains(const float3 point) const
    {
        return all(point > min) && all(point < max);
    }
    float3 Cuboid::Extrudes(const float3 point)
    {
        float3 halfSize = GetSize() * 0.5f;
        float3 localPoint = (point - GetCenter()) / halfSize;
        float3 localLength = abs(localPoint);

        if (localLength.x > localLength.y)
        {
            if (localLength.x > localLength.z)
                return {localPoint.x > 1 ? max.x : min.x, point.y, point.z};
            else
                return {point.x, point.y, localPoint.z > 1 ? max.z : min.z};
        }
        else
        {
            if (localLength.y > localLength.z)
                return {point.x, localPoint.y > 1 ? max.y : min.y, point.z};
            else
                return {point.x, point.y, localPoint.z > 1 ? max.z : min.z};
        }
    }
}