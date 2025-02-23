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

        void SetIsEnabled(bool state);

    private:
        Gleam_MakeType_Friend

        bool isEnabled = true;

        void Update() override;
    };
    Gleam_MakeGlobalSystem(MassSpringRenderingSystem)

    Gleam_MakeType(MassSpringRenderingSystem_T, "")
    {
        Gleam_MakeType_AddField(isEnabled);
    }
}