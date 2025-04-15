#pragma once
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    class GizmosSystem : public RelativeSystem<RenderingSystem, SystemRelation::Before>
    {
        friend class GizmosSystem_PostProcess;

        void Update() override;
    };
    Gleam_MakeEditorSystem(GizmosSystem)

    class GizmosSystem_PostProcess : public RelativeSystem<PresentationSystem, SystemRelation::After>
    {
        void Update() override;
    };
    Gleam_MakeEditorSystem(GizmosSystem_PostProcess);
}