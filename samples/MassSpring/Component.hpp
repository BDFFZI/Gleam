#pragma once
#include "LightECS/Runtime/Archetype.hpp"
#include "LightECS/Runtime/Entity.hpp"
#include "LightMath/Runtime/Vector.hpp"

struct Transform
{
    float2 position;
};

struct Point
{
    float4 color = 1;
};

struct Line
{
    Entity pointA = Entity::Null;
    Entity pointB = Entity::Null;
    float4 color = 1;
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

MakeArchetype(pointArchetype, Transform, Point)
MakeArchetype(lineArchetype, Line)
