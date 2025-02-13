#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    class Sphere
    {
    public:
        Sphere(float3 center, float radius);

        float3 GetCenter() const;
        float GetRadius() const;

        bool Contains(float3 point) const;
        float3 Extrudes(float3 point) const;

    private:
        float3 center;
        float radius;
    };
}