#include "Rectangle.h"

#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"


namespace Gleam
{
    Rectangle Rectangle::Zero = {};

    Rectangle::Rectangle()
        : x(0), y(0), width(0), height(0)
    {
    }
    Rectangle::Rectangle(const float x, const float y, const float width, const float height)
        : x(x), y(y), width(width), height(height)
    {
    }
    Rectangle::Rectangle(const float2 position, const float2 size)
        : x(position.x), y(position.y), width(size.x), height(size.y)
    {
    }
    Rectangle::operator float4() const
    {
        return {x, y, width, height};
    }
    bool Rectangle::operator==(const Rectangle rect) const
    {
        return all(static_cast<float4>(*this) == static_cast<float4>(rect));
    }

    void Rectangle::SetSize(const float2 size)
    {
        width = size.x;
        height = size.y;
    }
    void Rectangle::SetPosition(const float2 position)
    {
        x = position.x;
        y = position.y;
    }

    bool Rectangle::Contains(const float2 point) const
    {
        return point.x >= x && point.x - x <= width
            && point.y >= y && point.y - y <= height;
    }
}