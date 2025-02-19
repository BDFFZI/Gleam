#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    struct MassPointPhysics
    {
        float mass = 1;
        float drag = 0.01f; //避免因牛顿第一定律导致的始终匀速运动
        float3 force = 0;
    };

    struct MassPointLastState
    {
        float3 lastPosition = 0;
        float3 lastForce = 0;
    };

    Gleam_MakeType(MassPointPhysics, "3BF4A03E-EF37-4003-871A-6A5A03CCBCCF")
    {
        Gleam_MakeType_AddField(mass);
        Gleam_MakeType_AddField(drag);
        Gleam_MakeType_AddField(force);
    }

    Gleam_MakeType(MassPointLastState, "ABB2C900-1E9D-412D-A1E1-3D1F96533192")
    {
        Gleam_MakeType_AddField(lastPosition);
        Gleam_MakeType_AddField(lastForce);
    }
}