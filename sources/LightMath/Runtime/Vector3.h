#pragma once
#include <string>

#include "Math.h"
#include "Vector2.h"

struct Vector3
{
    static Vector3 zero;
    static Vector3 one;
    static Vector3 left;
    static Vector3 right;
    static Vector3 up;
    static Vector3 down;
    static Vector3 front;
    static Vector3 back;

    static Vector3 Lerp(Vector3 origin, Vector3 destination, float rate);
    static float Dot(Vector3 left, Vector3 right);
    static Vector3 Cross(Vector3 left, Vector3 right);
    static float Angle(Vector3 left, Vector3 right);
    /**
     * 
     * @param vector 
     * @param normal 请确保是单位向量
     * @return 
     */
    static Vector3 Project(Vector3 vector, Vector3 normal);
    /**
     * 
     * @param vector 
     * @param normal 请确保是单位向量
     * @param angle 
     * @return 
     */
    static Vector3 Rotate(Vector3 vector, Vector3 normal, float angle);


    float x;
    float y;
    float z;

    Vector3() = default;
    Vector3(float value);
    Vector3(float x, float y, float z);
    Vector3(Vector2 xy, float z);

    float GetSqrMagnitude() const;
    float GetMagnitude() const;
    Vector3 GetNormalized() const;
};

std::string to_string(Vector3 value);
Vector3 operator+(Vector3 left, Vector3 right);
Vector3 operator-(Vector3 left, Vector3 right);
Vector3 operator*(Vector3 left, Vector3 right);
Vector3 operator/(Vector3 left, Vector3 right);
bool operator==(Vector3 left, Vector3 right);

template <>
struct std::hash<Vector3>
{
    size_t operator()(Vector3 const& value) const noexcept
    {
        size_t seed = 0;
        hash<float> hasher;
        CombineHash(seed, hasher(value.x));
        CombineHash(seed, hasher(value.y));
        CombineHash(seed, hasher(value.z));
        return seed;
    }
};
