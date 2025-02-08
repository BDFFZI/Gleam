#pragma once
#include "LightMath/Runtime/LinearAlgebra/Vector.h"

namespace Light
{
    struct Rect
    {
        static Rect Zero;

        float x, y;
        float width, height;

        Rect();
        Rect(float x, float y, float width, float height);
        Rect(float2 position, float2 size);
        explicit operator float4() const;
        bool operator==(Rect rect) const;

        float2 GetPosition() const { return {x, y}; }
        float2 GetSize() const { return {width, height}; }
        float2 GetMin() const { return {x, y}; }
        float2 GetMax() const { return {x + width, y + height}; }
        float GetXMin() const { return x; }
        float GetYMin() const { return y; }
        float GetXMax() const { return x + width; }
        float GetYMax() const { return y + height; }
        void SetSize(float2 size);
        void SetPosition(float2 position);

        bool Contains(float2 point) const;
    };

    Light_MakeType(Rect, "CAD2E68E-EA4E-4848-9201-71CA36865003")
    {
        Light_MakeType_AddField(x);
        Light_MakeType_AddField(y);
        Light_MakeType_AddField(width);
        Light_MakeType_AddField(height);
    }
}