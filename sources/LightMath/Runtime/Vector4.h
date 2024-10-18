#pragma once
#include "Vector3.h"

struct Vector4
{
    float x;
    float y;
    float z;
    float w;

    Vector4() = default;
    Vector4(float value);
    Vector4(float x, float y, float z, float w);
    Vector4(Vector3 xyz, float w);
};

std::string to_string(Vector4 value);
bool operator==(Vector4 left, Vector4 right);