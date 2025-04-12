#pragma once
#include "RenderingSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    class PointsRendererSystem : public System
    {
    public:
        PointsRendererSystem(): System(GlobalRenderingSystem, SystemRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeRuntimeSystem(PointsRendererSystem)
}