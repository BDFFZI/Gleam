#pragma once
#include "PhysicsSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class ForceSystem : public System
    {
    public:
        ForceSystem(): System(PhysicsSystem, LeftOrder, MiddleOrder)
        {
        }

        void Update() override;
    };
    Gleam_MakeSystemInstance(ForceSystem);
}