#pragma once
#include <string>
#include "Vector.h"

namespace LightRuntime
{
    float2 operator+(float2 left, float2 right);
    float2 operator-(float2 left, float2 right);
    float2 operator*(float2 left, float2 right);
    float2 operator/(float2 left, float2 right);
    
    std::string to_string(float2 value);
}
