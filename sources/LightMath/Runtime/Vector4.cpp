#include "Vector4.h"

#include <format>

Vector4::Vector4(const float value)
{
    x = value;
    y = value;
    z = value;
    w = value;
}
Vector4::Vector4(const float x, const float y, const float z, const float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}
Vector4::Vector4(const Vector3 xyz, const float w)
{
    x = xyz.x;
    y = xyz.y;
    z = xyz.z;
    this->w = w;
}

std::string to_string(const Vector4 value)
{
    return std::format("({},{},{},{})", value.x, value.y, value.z, value.w);
}