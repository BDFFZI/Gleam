#pragma once
#include "RenderingSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class LinesRendererSystem : public System
    {
    public:
        LinesRendererSystem(): System(RenderingSystem, OrderRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeSystemInstance(LinesRendererSystem)
}