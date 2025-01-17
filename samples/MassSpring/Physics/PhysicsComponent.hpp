#pragma once
#include "LightECS/Runtime/Reference.hpp"
#include "LightMath/Runtime/Vector.hpp"
#include "LightEngine/Runtime/Reflection/Type.hpp"
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

    Light_MakeType("", MassPointPhysics)
    {
        Light_MakeType_AddField(force);
        Light_MakeType_AddField(velocity);
        Light_MakeType_AddField(mass);
        Light_MakeType_AddField(drag);
    }

    struct SpringPhysics
    {
        Reference<Point, MassPointPhysics> pointA;
        Reference<Point, MassPointPhysics> pointB;
        float length = 20;
        float elasticity = 200;
        float resistance = 2;
    };

    
    Light_MakeType("", SpringPhysics)
    {
        Light_MakeType_AddField(pointA);
        Light_MakeType_AddField(pointB);
        Light_MakeType_AddField(length);
        Light_MakeType_AddField(elasticity);
        Light_MakeType_AddField(resistance);
    }

}
