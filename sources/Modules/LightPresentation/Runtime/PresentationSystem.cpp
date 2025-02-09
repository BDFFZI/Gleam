#include "PresentationSystem.h"
#include "LightGraphics/Runtime/SwapChain.h"

void Light::PresentationSystem::Start()
{
    presentGCommandBuffer = std::make_unique<GCommandBuffer>("PresentGCommandBuffer");

    SystemGroup::Start();
}
void Light::PresentationSystem::Stop()
{
    SystemGroup::Stop();
    
    presentGCommandBuffer.reset();
}
void Light::PresentationSystem::Update()
{
    //开始呈现并录制呈现命令缓冲区
    if (SwapChain::BeginPresent(&presentGLCommandBuffer))
    {
        presentGCommandBuffer->BeginRecording(); //开始公共命令缓冲区录制
        SystemGroup::Update();
        presentGCommandBuffer->EndRecording(); //完成公共命令缓冲区录制
        presentGLCommandBuffer->ExecuteSubCommands(presentGCommandBuffer->GetGLCommandBuffer()); //执行公共缓冲区中的命令

        //结束呈现命令缓冲区录制并提交呈现命令
        SwapChain::EndPresent(*presentGLCommandBuffer);
        //等待呈现完成
        SwapChain::WaitPresent();
    }
}