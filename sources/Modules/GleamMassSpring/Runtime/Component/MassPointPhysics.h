#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    struct MassPointPhysics
    {
        float3 force;
        float3 velocity;
        float mass = 1;
        float drag = 0.01f;
    };

    Gleam_MakeType(MassPointPhysics, "3BF4A03E-EF37-4003-871A-6A5A03CCBCCF")
    {
        Gleam_MakeType_AddField(force);
        Gleam_MakeType_AddField(velocity);
        Gleam_MakeType_AddField(mass);
        Gleam_MakeType_AddField(drag);
    }
}