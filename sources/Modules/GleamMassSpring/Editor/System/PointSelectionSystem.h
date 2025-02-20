#pragma once
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamRendering/Runtime/System/CameraSystem.h"

namespace Gleam
{
    class PointSelectionSystem : public System
    {
    public:
        PointSelectionSystem(): System(EditorUISystem, MaxOrder, MaxOrder)
        {
        }

    private:
        Gleam_MakeType_Friend

        Entity optionalEntity = Entity::Null;
        float optionalEntityZ = 1;

        void Update() override;
    };
    Gleam_MakeGlobalSystem(PointSelectionSystem)

    Gleam_MakeType(PointSelectionSystem_T, "")
    {
        Gleam_MakeType_AddField(optionalEntity);
    }
}