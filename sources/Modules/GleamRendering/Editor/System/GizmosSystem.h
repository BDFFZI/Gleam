#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    class GizmosSystem : public System
    {
    public:
        static GizmosSystem& GetInstance();

    private:
        Gleam_Engine_Friend

        SystemEvent postProcessSystem = SystemEvent("GizmosSystem_PostProcess", GlobalPresentationSystem, OrderRelation::After);

        GizmosSystem(): System(GlobalRenderingSystem, OrderRelation::Before)
        {
        }
        void Start() override;
        void Stop() override;
        void Update() override;
    };
}