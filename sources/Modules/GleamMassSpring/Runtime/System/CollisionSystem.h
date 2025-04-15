#pragma once
#include "ForceSystem.h"

namespace Gleam
{
    class CollisionSystem : public RelativeSystem<ForceSystem, SystemRelation::After>
    {
        void Update() override;
    };
    Gleam_MakeRuntimeSystem(CollisionSystem)
}