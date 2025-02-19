#include "Point.h"

namespace Gleam
{
    Point::Point() = default;
    Point::Point(const float3 position)
        : position(position)
    {
    }
    Point::Point(const float x, const float y, const float z)
        : position(x, y, z)
    {
    }
}