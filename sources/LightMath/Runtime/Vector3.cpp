#include "Vector3.h"

#include <cmath>
#include <format>

#include "Math.h"

Vector3 Vector3::zero = 0;
Vector3 Vector3::one = 1;
Vector3 Vector3::left = {-1, 0, 0};
Vector3 Vector3::right = {1, 0, 0};
Vector3 Vector3::up = {0, 1, 0};
Vector3 Vector3::down = {0, -1, 0};
Vector3 Vector3::front = {0, 0, 1};
Vector3 Vector3::back = {0, 0, -1};

Vector3 Vector3::Lerp(const Vector3 origin, const Vector3 destination, const float rate)
{
    return origin + rate * (destination - origin);
}
float Vector3::Dot(const Vector3 left, const Vector3 right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z;
}
Vector3 Vector3::Cross(const Vector3 left, const Vector3 right)
{
    return {
        left.y * right.z - left.z * right.y,
        -(left.x * right.z - left.z * right.x),
        left.x * right.y - left.y * right.x
    };
}
Vector3 Vector3::Project(const Vector3 vector, const Vector3 normal)
{
    return normal * Dot(vector, normal);
}
float Vector3::Angle(const Vector3 left, const Vector3 right)
{
    return acos(Dot(left, right) / sqrt(left.GetSqrMagnitude() * right.GetSqrMagnitude())) * Rad2Deg;
}
Vector3 Vector3::Rotate(const Vector3 vector, const Vector3 normal, const float angle)
{
    float radian = angle * Deg2Rad;
    Vector3 parallel = Project(vector, normal);
    Vector3 perpendicular = vector - parallel;
    return cos(radian) * perpendicular + sin(radian) * Cross(normal, perpendicular) + parallel;
}

Vector3::Vector3(const float value)
{
    x = value;
    y = value;
    z = value;
}
Vector3::Vector3(const float x, const float y, const float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
Vector3::Vector3(const Vector2 xy, const float z)
{
    x = xy.x;
    y = xy.y;
    this->z = z;
}

float Vector3::GetSqrMagnitude() const
{
    return x * x + y * y + z * z;
}
float Vector3::GetMagnitude() const
{
    return sqrt(GetSqrMagnitude());
}
Vector3 Vector3::GetNormalized() const
{
    float sqrMagnitude = GetSqrMagnitude();
    return sqrMagnitude > FLT_EPSILON ? *this / sqrt(sqrMagnitude) : 0;
}

std::string to_string(const Vector3 value)
{
    return std::format("({:f},{:f},{:f})", value.x, value.y, value.z);
}
#define Vector3Operation(operation) Vector3 operator##operation(const Vector3 left, const Vector3 right)\
{\
return {\
left.x operation right.x,\
left.y operation right.y,\
left.z operation right.z,\
};\
}
Vector3Operation(+)
Vector3Operation(-)
Vector3Operation(*)
Vector3Operation(/)
bool operator==(const Vector3 left, const Vector3 right)
{
    float a = left.x - right.x;
    float b = left.y - right.y;
    float c = left.z - right.z;
    return a * a + b * b + c * c < SqrEPSILON * 3;
}
