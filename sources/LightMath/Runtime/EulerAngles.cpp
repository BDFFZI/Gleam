#include "EulerAngles.h"

#include <format>

#include "Math.h"

EulerAngles::EulerAngles(const float x, const float y, const float z)
    : x(x), y(y), z(z)
{
}
EulerAngles::EulerAngles(const float value)
    : x(value), y(value), z(value)
{
}
EulerAngles EulerAngles::GetNormalized() const
{
    return {
        fmod(fmod(x, 360.0f) + 360, 360.0f),
        fmod(fmod(y, 360.0f) + 360, 360.0f),
        fmod(fmod(z, 360.0f) + 360, 360.0f),
    };
}
std::string to_string(const EulerAngles value)
{
    return std::format("({:f},{:f},{:f})", value.x, value.y, value.z);
}
#define Vector3Operation(operation) EulerAngles operator##operation(const EulerAngles left, const EulerAngles right)\
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
bool operator==(const EulerAngles left, const EulerAngles right)
{
    float a = left.x - right.x;
    float b = left.y - right.y;
    float c = left.z - right.z;
    return a * a + b * b + c * c < SqrEPSILON * 3;
}
