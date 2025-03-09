#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    class GizmosSystem : public System
    {
    public:
        GizmosSystem(): System(GlobalRenderingSystem, OrderRelation::Before)
        {
        }

    private:
        SystemEvent postProcessSystem = SystemEvent("GizmosSystem_PostProcess", GlobalPresentationSystem, OrderRelation::After);

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(GizmosSystem)
}