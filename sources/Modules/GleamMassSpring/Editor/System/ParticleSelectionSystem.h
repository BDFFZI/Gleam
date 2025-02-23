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
        ParticleSelectionSystem(): System(EditorUISystem, MaxOrder, MaxOrder)
        {
        }

    private:
        Gleam_MakeType_Friend

        Entity optionalEntity = Entity::Null;
        float optionalEntityZ = 1;

        void Update() override;
    };
    Gleam_MakeGlobalSystem(ParticleSelectionSystem)

    Gleam_MakeType(ParticleSelectionSystem_T, "")
    {
        Gleam_MakeType_AddField(optionalEntity);
    }
}