#include "Vector2.h"

#include <format>

#include "Math.h"

Vector2::Vector2(const float value)
{
    x = value;
    y = value;
}
Vector2::Vector2(const float x, const float y)
{
    this->x = x;
    this->y = y;
}

std::string to_string(const Vector2 value)
{
    return std::format("({:f},{:f})", value.x, value.y);
}
#define Vector2Operation(operation) Vector2 operator##operation(const Vector2 left, const Vector2 right)\
{\
return {\
left.x operation right.x,\
left.y operation right.y,\
};\
}
Vector2Operation(+)
Vector2Operation(-)
Vector2Operation(*)
Vector2Operation(/)
bool operator==(const Vector2 left, const Vector2 right)
{
    float a = left.x - right.x;
    float b = left.y - right.y;
    return a * a + b * b < SqrEPSILON * 2;
}
