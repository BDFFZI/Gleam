#pragma once

namespace Gleam
{
    struct Collider
    {
        float friction = 0.5f;
        float elasticity = 0.5f;
    };

    Gleam_MakeType(Collider, "55833BC7-C3B8-4243-8EDE-A558F1856434")
    {
        Gleam_MakeType_AddField(friction);
        Gleam_MakeType_AddField(elasticity);
    }
}