#pragma once
#include "PhysicsSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class ForceSystem : public System
    {
    public:
        ForceSystem(): System(PhysicsSystem)
        {
        }

        void Update() override;
    };
    Gleam_MakeGlobalSystem(ForceSystem);
}