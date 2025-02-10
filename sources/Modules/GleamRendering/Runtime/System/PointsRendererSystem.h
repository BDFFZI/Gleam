#pragma once
#include "RenderingSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class PointsRendererSystem : public System
    {
    public:
        PointsRendererSystem(): System(RenderingSystem, OrderRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeSystemInstance(PointsRendererSystem)
}