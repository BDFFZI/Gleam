#include <gtest/gtest.h>

#include "LightEngine/Runtime/Engine.h"
#include "LightPresentation/Runtime/PresentationSystem.h"
#include "LightGraphics/Runtime/SwapChain.h"

class MySystem : public Light::System
{
public:
    MySystem(): System(Light::PresentationSystem)
    {
    }

private:
    void Update() override
    {
        Light::PresentationSystem->GetCommandBuffer().SetRenderTarget(Light::SwapChain::GetPresentRenderTarget());
        Light::PresentationSystem->GetCommandBuffer().ClearRenderTarget(Light::float4{0, 0, 1, 1});
    }
};
Light_MakeSystem(MySystem)
Light_AddSystems(MySystem)

TEST(Presentation, Presentation)
{
    Light::Engine::Start();
}