#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    struct MassPointPhysics
    {
        float3 lastPosition = 0;
        float3 lastForce = 0;
        float mass = 1;
        float drag = 0;
        float3 force = 0;
    };

    Gleam_MakeType(MassPointPhysics, "3BF4A03E-EF37-4003-871A-6A5A03CCBCCF")
    {
        Gleam_MakeType_AddField(lastPosition);
        Gleam_MakeType_AddField(mass);
        Gleam_MakeType_AddField(drag);
        Gleam_MakeType_AddField(force);
    }
}