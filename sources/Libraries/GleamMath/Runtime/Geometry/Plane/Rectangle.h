#pragma once
#include <string>

#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    class Rectangle
    {
    public:
        static Rectangle CreateFromOrigin(float2 origin, float2 size);
        static Rectangle CreateFromCenter(float2 center, float2 size);
        static Rectangle Create(float x, float y, float width, float height);

        Rectangle();
        Rectangle(float2 min, float2 max);
        bool operator==(Rectangle rect) const;

        float2 GetSize() const;
        float2 GetMin() const;
        float2 GetMax() const;
        void SetMin(float2 position);
        void SetMax(float2 position);
        void SetSize(float2 size);

        bool Contains(float2 point) const;

    private:
#ifdef GleamReflectionRuntime
        Gleam_MakeType_Friend
#endif
        
        float2 min;
        float2 max;
    };

    std::string to_string(const Rectangle& rectangle);

#ifdef GleamReflectionRuntime
    Gleam_MakeType(Rectangle, "CAD2E68E-EA4E-4848-9201-71CA36865003")
    {
        Gleam_MakeType_AddField(min);
        Gleam_MakeType_AddField(max);
    }
#endif
}