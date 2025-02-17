#pragma once

namespace Gleam
{
    struct Collider
    {
        float friction;
        float elasticity;
    };

    Gleam_MakeType(Collider,"55833BC7-C3B8-4243-8EDE-A558F1856434")
    {
        Gleam_MakeType_AddField(friction);
        Gleam_MakeType_AddField(elasticity);
    }
}