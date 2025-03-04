#pragma once

#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"

#ifdef GleamReflectionRuntime
#include "GleamReflection/Runtime/Type.h"
#endif

namespace Gleam
{
    struct Rectangle
    {
        static Rectangle CreateFromOrigin(const float2 origin, const float2 size)
        {
            return {origin, origin + size};
        }
        static Rectangle CreateFromCenter(const float2 center, const float2 size)
        {
            float2 halfSize = size / 2;
            return {center - halfSize, center + halfSize};
        }
        static Rectangle Create(float x, float y, const float width, const float height)
        {
            return {{x, y}, {x + width, y + height}};
        }

        float2 min;
        float2 max;

        bool operator==(Rectangle rect) const;

        float2 GetSize() const { return max - min; }
        void SetSize(const float2 size) { max = min + size; }
    };

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Rectangle, "")
    {
        Gleam_MakeType_AddField(min);
        Gleam_MakeType_AddField(max);
    }
#endif
}