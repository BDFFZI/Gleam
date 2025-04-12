#pragma once
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"

namespace Gleam
{
    class MassSpringRenderingSystem : public System
    {
    public:
        MassSpringRenderingSystem(): System(GlobalPhysicsSystem, SystemRelation::After)
        {
        }

        void SetIsEnabled(bool state);

    private:
        Gleam_MakeType_Friend

        bool isEnabled = true;

        void Update() override;
    };
    Gleam_MakeTypeWithID(MassSpringRenderingSystem, "")
    {
        Gleam_MakeType_AddField(isEnabled);
    }

    Gleam_MakeRuntimeSystem(MassSpringRenderingSystem)
}