#pragma once
#include <string>

#include "Math.h"

struct Vector2
{
    float x;
    float y;

    Vector2() = default;
    Vector2(float value);
    Vector2(float x, float y);
};

std::string to_string(Vector2 value);
Vector2 operator+(Vector2 left, Vector2 right);
Vector2 operator-(Vector2 left, Vector2 right);
Vector2 operator*(Vector2 left, Vector2 right);
Vector2 operator/(Vector2 left, Vector2 right);
bool operator==(Vector2 left, Vector2 right);

template <>
struct std::hash<Vector2>
{
    size_t operator()(Vector2 const& value) const noexcept
    {
        size_t seed = 0;
        hash<float> hasher;
        CombineHash(seed, hasher(value.x));
        CombineHash(seed, hasher(value.y));
        return seed;
    }
};
