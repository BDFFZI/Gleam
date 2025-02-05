#pragma once
#include "LightMath/Runtime/LinearAlgebra/Vector.h"

namespace Light
{
    struct MassPointPhysics
    {
        float3 force;
        float3 velocity;
        float mass = 1;
        float drag = 0.99f;
    };

    Light_MakeType(MassPointPhysics, "3BF4A03E-EF37-4003-871A-6A5A03CCBCCF")
    {
        Light_MakeType_AddField(force);
        Light_MakeType_AddField(velocity);
        Light_MakeType_AddField(mass);
        Light_MakeType_AddField(drag);
    }
}