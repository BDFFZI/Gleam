#pragma once
#include "PositionSystem.h"

namespace Gleam
{
    class ForceSystem : public RelativeSystem<PositionSystem, SystemRelation::After>
    {
        void Update() override;
    };

    Gleam_MakeRuntimeSystem(ForceSystem);
}