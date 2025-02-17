#pragma once
#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamRendering/Runtime/System/CameraSystem.h"

namespace Gleam
{
    class PointSelector : public System
    {
    public:
        PointSelector(): System(InspectorWindow, OrderRelation::Before)
        {
        }

    private:
        Gleam_MakeType_Friend

        std::vector<Entity> optionalEntities;

        void Update() override;
    };
    Gleam_MakeGlobalSystem(PointSelector)

    Gleam_MakeType(PointSelector_T, "")
    {
        Gleam_MakeType_AddField(optionalEntities);
    }
}