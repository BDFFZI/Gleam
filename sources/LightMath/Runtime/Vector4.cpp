#include "Vector4.h"

#include <format>

#include "Math.h"

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
    return std::format("({:f},{:f},{:f},{:f})", value.x, value.y, value.z, value.w);
}
bool operator==(const Vector4 left, const Vector4 right)
{
    float a = left.x - right.x;
    float b = left.y - right.y;
    float c = left.z - right.z;
    float d = left.w - right.w;
    return a * a + b * b + c * c + d * d < SqrEPSILON * 4;
}
