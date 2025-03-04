#pragma once
#include "RenderingSystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class CameraSystem : public System
    {
    public:
        CameraSystem(): System(GlobalRenderingSystem, OrderRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(CameraSystem)
}