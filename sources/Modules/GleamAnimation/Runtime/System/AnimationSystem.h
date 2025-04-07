#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

namespace Gleam
{
    class AnimationSystem : public System
    {
    public:
        AnimationSystem(): System(GlobalPostUpdateSystem, MinOrder, DefaultOrder)
        {
        }
    private:
        void Update() override;
    };
    Gleam_MakeSystem(AnimationSystem)
}