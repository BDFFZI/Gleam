#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

namespace Gleam
{
    class AnimationSystem : public System<PostUpdateSystem, SystemMinOrder, SystemMidOrder>
    {
        void Update() override;
    };
    Gleam_MakeRuntimeSystem(AnimationSystem)
}