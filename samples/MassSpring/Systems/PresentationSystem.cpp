#include "PresentationSystem.h"

#include "LightGraphics/Runtime/SwapChain.h"
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
    //开始呈现并录制呈现命令缓冲区
    Presentation::canPresent = SwapChain::BeginPresent(&Presentation::presentCommandBuffer);
    //开始公共命令缓冲区录制
    Presentation::commandBuffer->BeginRecording();
}

void Light::EndPresentationSystem::Update()
{
    //完成公共命令缓冲区录制
    Presentation::commandBuffer->EndRecording();
    Presentation::presentCommandBuffer->ExecuteSubCommands(Presentation::commandBuffer->GetGLCommandBuffer());
    //结束呈现命令缓冲区录制并提交呈现命令
    if (Presentation::CanPresent())
        SwapChain::EndPresent(*Presentation::presentCommandBuffer);
    //等待上次呈现完成
    SwapChain::WaitPresent();
}
