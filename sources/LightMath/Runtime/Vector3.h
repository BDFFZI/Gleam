#pragma once
#include <string>

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

    static float Magnitude(Vector3 vector);
    static float SqrMagnitude(Vector3 vector);
    static Vector3 Normalize(Vector3 vector);
    static Vector3 Lerp(Vector3 origin, Vector3 destination, float rate);
    static float Dot(Vector3 left, Vector3 right);
    static Vector3 Cross(Vector3 left, Vector3 right);
    static Vector3 Angle(Vector3 left, Vector3 right);
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
     * @param radian 
     * @return 
     */
    static Vector3 Rotate(Vector3 vector, Vector3 normal, float radian);


    float x;
    float y;
    float z;

    Vector3() = default;
    Vector3(float value);
    Vector3(float x, float y, float z);
    Vector3(Vector2 xy, float z);
};

std::string to_string(Vector3 value);
Vector3 operator+(Vector3 left, Vector3 right);
Vector3 operator-(Vector3 left, Vector3 right);
Vector3 operator*(Vector3 left, Vector3 right);
Vector3 operator/(Vector3 left, Vector3 right);
bool operator==(Vector3 left, Vector3 right);

