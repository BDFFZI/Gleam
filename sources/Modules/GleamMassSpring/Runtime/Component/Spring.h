#pragma once
#include "Particle.h"
#include "GleamECS/Runtime/EntityRef.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"

namespace Gleam
{
    struct Spring
    {
        Entity particleA;
        Entity particleB;
        float length = 1;
        float elasticity = 0.5;
    };

    Gleam_MakeType(Spring, "F08A937F-7E67-47F9-AFE2-8C2A55AF204E")
    {
        Gleam_MakeType_AddField(particleA);
        Gleam_MakeType_AddField(particleB);
        Gleam_MakeType_AddField(length);
        Gleam_MakeType_AddField(elasticity);
    }
}