#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct Particle
    {
        float3 lastPosition = 0;
        float3 position = 0;
        float mass = 1; //使质点的速度更难以被改变
        float drag = 0.01f; //避免因能量守恒导致的始终匀速运动
    };

    Gleam_MakeType(Particle, "3BF4A03E-EF37-4003-871A-6A5A03CCBCCF")
    {
        Gleam_MakeType_AddField(lastPosition);
        Gleam_MakeType_AddField(position);
        Gleam_MakeType_AddField(mass);
        Gleam_MakeType_AddField(drag);
    }
}