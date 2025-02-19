#pragma once
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"

namespace Gleam
{
    class MassSpringRenderingSystem : public System
    {
    public:
        MassSpringRenderingSystem(): System(PhysicsSystem, OrderRelation::After)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(MassSpringRenderingSystem)
}