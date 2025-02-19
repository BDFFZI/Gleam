#pragma once
#include "MassPointPhysics.h"
#include "GleamECS/Runtime/EntityRef.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"

namespace Gleam
{
    struct SpringPhysics
    {
        Entity pointA;
        Entity pointB;
        float length = 10;
        float elasticity = 1000;
        float resistance = 0.1; //解决积分误差，使弹簧能够稳定并看起来更硬
    };

    Gleam_MakeType(SpringPhysics, "F08A937F-7E67-47F9-AFE2-8C2A55AF204E")
    {
        Gleam_MakeType_AddField(pointA);
        Gleam_MakeType_AddField(pointB);
        Gleam_MakeType_AddField(length);
        Gleam_MakeType_AddField(elasticity);
        Gleam_MakeType_AddField(resistance);
    }
}