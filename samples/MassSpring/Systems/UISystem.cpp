#include "UISystem.h"
#include "LightUI/Runtime/UI.h"

void Light::BeginUISystem::Update()
{
    UI::BeginFrame();
}
void Light::EndUISystem::Update()
{
    //完成UI绘图录制并执行
    Presentation::GetCommandBuffer().BeginRendering(Graphics::GetPresentRenderTarget());
    UI::EndFrame(Presentation::GetCommandBuffer().GetGLCommandBuffer());
    Presentation::GetCommandBuffer().EndRendering();
}
