﻿#pragma once
// #include "GleamECS/Runtime/EntityRef.h"
#include "MassPointPhysics.h"
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/EntityRef.h"
#include "GleamMath/Runtime/Geometry/Point.h"

namespace Gleam
{
    struct SpringPhysics
    {
        EntityRef<Point, MassPointPhysics> pointA;
        EntityRef<Point, MassPointPhysics> pointB;
        float length = 10;
        float elasticity = 200;
        float resistance = 2;
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