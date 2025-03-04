#pragma once
#include "RenderingSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class PointsRendererSystem : public System
    {
    public:
        PointsRendererSystem(): System(GlobalRenderingSystem, OrderRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(PointsRendererSystem)
}