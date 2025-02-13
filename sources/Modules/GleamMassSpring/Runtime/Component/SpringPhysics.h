#pragma once
#include "MassPointPhysics.h"
#include "GleamECS/Runtime/EntityRef.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"

namespace Gleam
{
    struct SpringPhysics
    {
        EntityRef<Point, MassPointPhysics> pointA;
        EntityRef<Point, MassPointPhysics> pointB;
        float length = 10;
        float elasticity = 3000;
    };
    
    Gleam_MakeType(SpringPhysics, "F08A937F-7E67-47F9-AFE2-8C2A55AF204E")
    {
        Gleam_MakeType_AddField(pointA);
        Gleam_MakeType_AddField(pointB);
        Gleam_MakeType_AddField(length);
        Gleam_MakeType_AddField(elasticity);
    }
}