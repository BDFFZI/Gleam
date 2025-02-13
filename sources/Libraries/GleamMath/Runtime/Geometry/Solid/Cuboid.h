#pragma once

#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    class Cuboid
    {
    public:
        static Cuboid Create(float3 center, float3 size);

        Cuboid();
        Cuboid(float3 min, float3 max);

        float3 GetMin() const;
        float3 GetMax() const;
        float3 GetCenter() const;
        float3 GetSize() const;

        bool Contains(float3 point) const;
        float3 Extrudes(float3 point);

    private:
        float3 min;
        float3 max;
    };
}