#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"

namespace Gleam
{
    struct Particle
    {
        float3 lastPosition = 0;
        float3 position = 0; //在Verlet积分中可以直接修改position来实现加速度
        float mass = 1;
        float drag = 0.01f; //避免因牛顿第一定律导致的始终匀速运动
    };

    Gleam_MakeType(Particle, "3BF4A03E-EF37-4003-871A-6A5A03CCBCCF")
    {
        Gleam_MakeType_AddField(lastPosition);
        Gleam_MakeType_AddField(position);
        Gleam_MakeType_AddField(mass);
        Gleam_MakeType_AddField(drag);
    }
}