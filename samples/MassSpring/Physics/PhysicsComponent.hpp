#pragma once
#include "LightECS/Runtime/Reference.hpp"
#include "LightMath/Runtime/Vector.hpp"
#include "LightReflection/Runtime/Type.hpp"
#include "../Public/Component.hpp"

namespace Light
{
    struct MassPointPhysics
    {
        float2 force;
        float2 velocity;
        float mass = 1;
        float drag = 0.99f;
    };

    MakeType("", MassPointPhysics)
    {
        MakeType_AddField(force);
        MakeType_AddField(velocity);
        MakeType_AddField(mass);
        MakeType_AddField(drag);
    }

    struct SpringPhysics
    {
        Reference<Point, MassPointPhysics> pointA;
        Reference<Point, MassPointPhysics> pointB;
        float length = 20;
        float elasticity = 200;
        float resistance = 2;
    };

    
    MakeType("", SpringPhysics)
    {
        MakeType_AddField(pointA);
        MakeType_AddField(pointB);
        MakeType_AddField(length);
        MakeType_AddField(elasticity);
        MakeType_AddField(resistance);
    }

}
