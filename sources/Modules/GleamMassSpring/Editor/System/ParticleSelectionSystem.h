#pragma once
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamRendering/Runtime/System/CameraSystem.h"

namespace Gleam
{
    class ParticleSelectionSystem : public System
    {
    public:
        ParticleSelectionSystem(): System(GlobalEditorUISystem, MaxOrder, MaxOrder)
        {
        }

    private:
        Gleam_MakeType_Friend

        Entity inspectingEntity = Entity::Null;
        Entity optionalEntity = Entity::Null;
        float optionalEntityZ = 1;

        void Update() override;
    };
    Gleam_MakeType(ParticleSelectionSystem, "")
    {
        Gleam_MakeType_AddField(optionalEntity);
    }

    Gleam_MakeGlobalSystem(ParticleSelectionSystem)
}