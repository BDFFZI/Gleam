#pragma once
#include "LightECS/Runtime/Entity.hpp"
#include "LightMath/Runtime/Vector.hpp"

struct Transform
{
    float2 position;
};

struct Point
{
    float4 color;
};

struct Line
{
    Entity pointA;
    Entity pointB;
    float4 color;
};

struct RigidBody
{
    float mass = 1;
    float2 force;
    float2 velocity;
};

struct Spring
{
    float2 pinPosition = 0;
    float length = 5;
    float elasticity = 200;
};
