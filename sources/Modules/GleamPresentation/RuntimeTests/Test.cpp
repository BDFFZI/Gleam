#include <gtest/gtest.h>

#include "GleamEngine/Runtime/Engine.h"
#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"

class MySystem : public Gleam::System<Gleam::PresentationSystem>
{
    void Update() override
    {
        Gleam::GlobalPresentationSystem->GetPresentGCommandBuffer().SetRenderTarget(Gleam::SwapChain::GetPresentRenderTarget());
        Gleam::GlobalPresentationSystem->GetPresentGCommandBuffer().ClearRenderTarget(Gleam::float4{0, 0, 1, 1});
    }
};
Gleam_MakeRuntimeSystem(MySystem)

Gleam_Main