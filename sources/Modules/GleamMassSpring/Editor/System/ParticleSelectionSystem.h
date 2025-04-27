#pragma once
#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamRendering/Runtime/System/CameraPreProcessSystem.h"

namespace Gleam
{
    class ParticleSelectionSystem : public AbsoluteSystem<EditorUISystem, SystemMaxOrder>
    {
        Gleam_MakeType_Friend

        Entity optionalEntity = Entity::Null;
        float optionalEntityZ = 1;

        void Update() override;
    };
    Gleam_MakeTypeWithID(ParticleSelectionSystem, "")
    {
        Gleam_MakeType_AddField(optionalEntity);
    }
    Gleam_MakeEditorSystem(ParticleSelectionSystem)
}