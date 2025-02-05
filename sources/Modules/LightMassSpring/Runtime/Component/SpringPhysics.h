#pragma once
// #include "LightECS/Runtime/EntityRef.h"
#include "MassPointPhysics.h"
#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/EntityRef.h"
#include "LightMath/Runtime/Geometry/Point.h"

namespace Light
{
    struct SpringPhysics
    {
        EntityRef<Point, MassPointPhysics> pointA;
        EntityRef<Point, MassPointPhysics> pointB;
        float length = 20;
        float elasticity = 200;
        float resistance = 2;
    };
    
    Light_MakeType(SpringPhysics, "F08A937F-7E67-47F9-AFE2-8C2A55AF204E")
    {
        Light_MakeType_AddField(pointA);
        Light_MakeType_AddField(pointB);
        Light_MakeType_AddField(length);
        Light_MakeType_AddField(elasticity);
        Light_MakeType_AddField(resistance);
    }
}