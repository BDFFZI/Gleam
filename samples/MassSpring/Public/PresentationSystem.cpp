#include "PresentationSystem.h"

#include "LightGraphics/Runtime/SwapChain.h"
#include "LightUI/Runtime/UI.h"

void Light::PresentationSystem::Start()
{
    presentGCommandBuffer = &Graphics::ApplyCommandBuffer();
    SystemGroup::Start();
}
void Light::PresentationSystem::Stop()
{
    SystemGroup::Stop();
    Graphics::ReleaseCommandBuffer(*presentGCommandBuffer);
}
void Light::PresentationSystem::Update()
{
    //开始呈现并录制呈现命令缓冲区
    if (SwapChain::BeginPresent(&presentGLCommandBuffer))
    {
        //开始公共命令缓冲区录制
        presentGCommandBuffer->BeginRecording();

        SystemGroup::Update();

        //完成公共命令缓冲区录制
        presentGCommandBuffer->EndRecording();
        //执行公共缓冲区中的命令
        presentGLCommandBuffer->ExecuteSubCommands(presentGCommandBuffer->GetGLCommandBuffer());
        //结束呈现命令缓冲区录制并提交呈现命令
        SwapChain::EndPresent(*presentGLCommandBuffer);
        //等待呈现完成
        SwapChain::WaitPresent();
    }
}
