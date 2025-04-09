#pragma once
#include "RenderingSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    class LinesRendererSystem : public System
    {
    public:
        LinesRendererSystem(): System(GlobalRenderingSystem, SystemRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeSystem(LinesRendererSystem)
}