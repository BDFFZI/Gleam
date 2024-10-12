#pragma once
#include <string>

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