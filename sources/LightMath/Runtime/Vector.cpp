#include "Vector.h"

using namespace LightRuntime;

float2::float2(const float value)
{
    x = value;
    y = value;
}
float2::float2(const float x, const float y)
{
    this->x = x;
    this->y = y;
}
float3::float3(const float value)
{
    x = value;
    y = value;
    z = value;
}
float3::float3(const float x, const float y, const float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
float3::float3(const float2 xy, const float z)
{
    x = xy.x;
    y = xy.y;
    this->z = z;
}
float4::float4(const float value)
{
    x = value;
    y = value;
    z = value;
    w = value;
}
float4::float4(const float x, const float y, const float z, const float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}
float4::float4(const float3 xyz, const float w)
{
    x = xyz.x;
    y = xyz.y;
    z = xyz.z;
    this->w = w;
}
