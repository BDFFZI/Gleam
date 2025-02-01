#include "Rect.h"

#include "LightMath/Runtime/LinearAlgebra/VectorMath.h"


namespace Light
{
    Rect Rect::Zero = {};

    Rect::Rect()
        : x(0), y(0), width(0), height(0)
    {
    }
    Rect::Rect(const float x, const float y, const float width, const float height)
        : x(x), y(y), width(width), height(height)
    {
    }
    Rect::Rect(const float2 position, const float2 size)
        : x(position.x), y(position.y), width(size.x), height(size.y)
    {
    }
    Rect::operator float4() const
    {
        return {x, y, width, height};
    }
    bool Rect::operator==(const Rect rect) const
    {
        return all(static_cast<float4>(*this) == static_cast<float4>(rect));
    }

    void Rect::SetSize(const float2 size)
    {
        width = size.x;
        height = size.y;
    }
    void Rect::SetPosition(const float2 position)
    {
        x = position.x;
        y = position.y;
    }

    bool Rect::Contains(const float2 point) const
    {
        return point.x >= x && point.x - x <= width
            && point.y >= y && point.y - y <= height;
    }
}