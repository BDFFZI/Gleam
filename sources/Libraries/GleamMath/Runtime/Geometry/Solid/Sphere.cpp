#include "Sphere.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

namespace Gleam
{
    Sphere::Sphere(float3 center, float radius)
        : center(center), radius(radius)
    {
    }
    float3 Sphere::GetCenter() const
    {
        return center;
    }
    float Sphere::GetRadius() const
    {
        return radius;
    }
    bool Sphere::Contains(const float3 point) const
    {
        float3 localPoint = point - center;
        return lengthsq(localPoint) < radius * radius;
    }
    float3 Sphere::Extrudes(float3 point) const
    {
        float3 localPoint = point - center;
        float3 direction = normalize(localPoint);
        return center + direction * radius;
    }
}