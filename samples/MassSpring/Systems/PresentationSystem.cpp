#include "PresentationSystem.h"
#include "LightUI/Runtime/UI.h"

void Light::BeginPresentationSystem::Start()
{
    Presentation::commandBuffer = &Graphics::ApplyCommandBuffer();
}
void Light::BeginPresentationSystem::Stop()
{
    Graphics::ReleaseCommandBuffer(*Presentation::commandBuffer);
}
void Light::BeginPresentationSystem::Update()
{
    //开始呈现命令缓冲区录制
    Presentation::presentCommandBuffer = &Graphics::BeginPresent();
    //开始公共命令缓冲区录制
    Presentation::commandBuffer->BeginRecording();
    //开始UI绘图录制
    UI::BeginFrame();
}

void Light::EndPresentationSystem::Update()
{
    //完成UI绘图录制并执行
    Presentation::commandBuffer->BeginRendering(Graphics::GetPresentRenderTexture());
    UI::EndFrame(Presentation::commandBuffer->GetGLCommandBuffer());
    Presentation::commandBuffer->EndRendering();
    //完成公共命令缓冲区录制并执行
    Presentation::commandBuffer->EndRecording();
    Presentation::presentCommandBuffer->ExecuteSubCommands(Presentation::commandBuffer->GetGLCommandBuffer());
    //完成呈现命令缓冲区录制并执行
    Graphics::EndPresent(*Presentation::presentCommandBuffer);

    //等待呈现完成
    Graphics::WaitPresent();
}

