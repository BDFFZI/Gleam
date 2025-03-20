#pragma once
#include "GleamAnimation/Runtime/Asset/AnimationClip.h"

namespace Gleam
{
    struct Animation
    {
        AnimationClip* clip;
        bool isPlaying;
        float time;
    };
    Gleam_MakeType(Animation, "9929918F-F9D3-40EF-B8F0-4D2BAF38A357")
    {
        Gleam_MakeType_AddField(clip);
        Gleam_MakeType_AddField(isPlaying);
        Gleam_MakeType_AddField(time);
    }
}