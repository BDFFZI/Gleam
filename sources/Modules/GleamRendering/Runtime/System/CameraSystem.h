#pragma once
#include "RenderingSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    class CameraSystem : public System
    {
    public:
        CameraSystem(): System(GlobalRenderingSystem, SystemRelation::Before)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeSystem(CameraSystem)
}