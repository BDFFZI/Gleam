#pragma once
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"

namespace Gleam
{
    class MassSpringRenderingSystem : public System
    {
    public:
        MassSpringRenderingSystem(): System(GlobalPhysicsSystem, OrderRelation::After)
        {
        }

        void SetIsEnabled(bool state);

    private:
        Gleam_MakeType_Friend

        bool isEnabled = true;

        void Update() override;
    };
    Gleam_MakeType(MassSpringRenderingSystem, "")
    {
        Gleam_MakeType_AddField(isEnabled);
    }

    Gleam_MakeGlobalSystem(MassSpringRenderingSystem)
}