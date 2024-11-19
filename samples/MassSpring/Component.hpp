#pragma once
#include "LightECS/Runtime/Archetype.hpp"
#include "LightECS/Runtime/Entity.hpp"
#include "LightMath/Runtime/Vector.hpp"

struct Point
{
    float2 position;
};

struct Line
{
    float2 positionA;
    float2 positionB;
};

struct Renderer
{
    float4 color = 1;
};

struct MassPointPhysics
{
    float2 force;
    float2 velocity;
    float mass = 1;
};

struct SpringPhysics
{
    Entity pointA = Entity::Null;
    Entity pointB = Entity::Null;
    float length = 5;
    float elasticity = 200;
};

MakeArchetype(massPointArchetype, Point, Renderer, MassPointPhysics)
MakeArchetype(springArchetype, Line, Renderer, SpringPhysics)
