#include "Rectangle.h"

#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"


namespace Gleam
{
    Rectangle Rectangle::CreateFromOrigin(float2 origin, const float2 size)
    {
        return {origin, origin + size};
    }
    Rectangle Rectangle::CreateFromCenter(const float2 center, const float2 size)
    {
        float2 halfSize = size / 2;
        return {center - halfSize, center + halfSize};
    }
    Rectangle Rectangle::Create(float x, float y, const float width, const float height)
    {
        return {{x, y}, {x + width, y + height}};
    }
    Rectangle::Rectangle()
        : min(0), max(0)
    {
    }
    Rectangle::Rectangle(const float2 min, const float2 max)
        : min(min), max(max)
    {
    }

    bool Rectangle::operator==(const Rectangle rect) const
    {
        return all(*reinterpret_cast<const float4*>(this) == *reinterpret_cast<const float4*>(&rect));
    }
    float2 Rectangle::GetSize() const
    {
        return max - min;
    }
    float2 Rectangle::GetMin() const
    {
        return min;
    }
    float2 Rectangle::GetMax() const
    {
        return max;
    }

    void Rectangle::SetMin(const float2 position)
    {
        min = position;
    }
    void Rectangle::SetMax(const float2 position)
    {
        max = position;
    }
    void Rectangle::SetSize(const float2 size)
    {
        max = min + size;
    }

    bool Rectangle::Contains(const float2 point) const
    {
        return all(point > min) && all(point < max);
    }
}