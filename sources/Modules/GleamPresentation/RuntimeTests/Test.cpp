#include <gtest/gtest.h>

#include "GleamECS/Runtime/World/World.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"

class MySystem : public Gleam::SystemT<Gleam::PresentationSystem>
{
    Gleam::PresentationSystem* presentationSystem = {};

    void Start() override
    {
        presentationSystem = Gleam::World::GetCurrentWorld().GetSystemAllocator().GetSystemPtr<Gleam::PresentationSystem>().lock().get();
    }
    void Update() override
    {
        presentationSystem->GetPresentGCommandBuffer().SetRenderTarget(Gleam::SwapChain::GetPresentRenderTarget());
        presentationSystem->GetPresentGCommandBuffer().ClearRenderTarget(Gleam::float4{0, 0, 1, 1});
    }
};
Gleam_MakeSystem(MySystem)
Gleam_AddRuntimeSystems(MySystem)

Gleam_Main