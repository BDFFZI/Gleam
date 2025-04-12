#pragma once
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    class GizmosSystem : public System
    {
    public:
        GizmosSystem(): System(GlobalRenderingSystem, SystemRelation::Before)
        {
        }

    private:
        SystemEvent postProcessSystem = SystemEvent("GizmosSystem_PostProcess", GlobalPresentationSystem, SystemRelation::After);

        void Start() override;
        void Stop() override;
        void Update() override;
    };
    Gleam_MakeRuntimeSystem(GizmosSystem)
}