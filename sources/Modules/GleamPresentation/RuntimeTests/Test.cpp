#include <gtest/gtest.h>

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"

class MySystem : public Gleam::System
{
public:
    MySystem(): System(Gleam::PresentationSystem)
    {
    }

private:
    void Update() override
    {
        Gleam::PresentationSystem->GetPresentGCommandBuffer().SetRenderTarget(Gleam::SwapChain::GetPresentRenderTarget());
        Gleam::PresentationSystem->GetPresentGCommandBuffer().ClearRenderTarget(Gleam::float4{0, 0, 1, 1});
    }
};
Gleam_MakeSystemInstance(MySystem)
Gleam_AddSystems(MySystem)

Gleam_Main