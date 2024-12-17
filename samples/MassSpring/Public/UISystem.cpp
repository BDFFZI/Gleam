#include "UISystem.h"

#include "LightGraphics/Runtime/SwapChain.h"
#include "LightUI/Runtime/UI.h"

void Light::UISystem::Update()
{
    UI::BeginFrame();
    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

    SystemGroup::Update();

    ImGui::ShowDemoWindow();

    //完成UI绘图录制并执行
    PresentationSystem.GetCommandBuffer().BeginRendering(SwapChain::GetPresentRenderTarget());
    UI::EndFrame(PresentationSystem.GetCommandBuffer().GetGLCommandBuffer());
    PresentationSystem.GetCommandBuffer().EndRendering();
}
