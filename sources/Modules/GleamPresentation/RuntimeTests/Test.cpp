#include <gtest/gtest.h>

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"

class MySystem : public Gleam::System
{
public:
    MySystem(): System(Gleam::GlobalPresentationSystem)
    {
    }

private:
    void Update() override
    {
        Gleam::GlobalPresentationSystem.GetPresentGCommandBuffer().SetRenderTarget(Gleam::SwapChain::GetPresentRenderTarget());
        Gleam::GlobalPresentationSystem.GetPresentGCommandBuffer().ClearRenderTarget(Gleam::float4{0, 0, 1, 1});
    }
};
Gleam_MakeGlobalSystem(MySystem)
Gleam_AddSystems(GlobalMySystem)

Gleam_Main