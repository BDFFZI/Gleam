#pragma once
#include <string>
#include "Vector.h"

namespace LightRuntime
{
    float2 operator+(float2 left, float2 right);
    float2 operator-(float2 left, float2 right);
    float2 operator*(float2 left, float2 right);
    float2 operator/(float2 left, float2 right);

    float3 operator+(float3 left, float3 right);
    float3 operator-(float3 left, float3 right);
    float3 operator*(float3 left, float3 right);
    float3 operator/(float3 left, float3 right);
    
    std::string to_string(float2 value);
}
