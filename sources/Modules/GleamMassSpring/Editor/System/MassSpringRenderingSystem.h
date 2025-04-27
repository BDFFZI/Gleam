#pragma once
#include "ParticleEditingSystem.h"
#include "GleamMassSpring/Runtime/System/PhysicsSystem.h"

namespace Gleam
{
    class MassSpringRenderingSystem : public RelativeSystem<PhysicsSystem, SystemRelation::After>
    {
    public:
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
    Gleam_MakeEditorSystem(MassSpringRenderingSystem)
}