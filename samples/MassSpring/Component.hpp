#pragma once
#include "LightMath/Runtime/Vector.hpp"

struct Transform
{
    float2 position;
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
