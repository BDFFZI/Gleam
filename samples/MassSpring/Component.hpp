#pragma once
#include "LightECS/Runtime/Entity.hpp"
#include "LightMath/Runtime/Vector.hpp"

struct Transform
{
    float2 position;
};

struct PointRender
{
     
};

struct Line
{
    Entity pointA;
    Entity pointB;
};

struct LineRender
{
    
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
