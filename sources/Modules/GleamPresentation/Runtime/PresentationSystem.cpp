#include "PresentationSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"

GLCommandBuffer& Gleam::PresentationSystem::GetPresentGLCommandBuffer() const
{
    return *presentGLCommandBuffer;
}
Gleam::GCommandBuffer& Gleam::PresentationSystem::GetPresentGCommandBuffer() const
{
    return *presentGCommandBuffer;
}

void Gleam::PresentationSystem::Start()
{
    presentGCommandBuffer = std::make_unique<GCommandBuffer>("PresentGCommandBuffer");

    SystemGroup::Start();
}
void Gleam::PresentationSystem::Stop()
{
    SwapChain::WaitPresent(); //等待上一帧结束，因为图形资源必须是不被占用的情况下才能销毁。

    SystemGroup::Stop();

    presentGCommandBuffer.reset();
}
void Gleam::PresentationSystem::Update()
{
    //等待上一帧渲染完毕（绘制时会修改图形资源，而因为资源是每帧共用，故必须确保上一帧已经用完了）
    SwapChain::WaitPresent();

    //开始呈现并录制呈现命令缓冲区
    if (SwapChain::BeginPresent(&presentGLCommandBuffer))
    {
        presentGCommandBuffer->BeginRecording(); //开始公共命令缓冲区录制
        SystemGroup::Update();
        presentGCommandBuffer->EndRecording(); //完成公共命令缓冲区录制
        presentGLCommandBuffer->ExecuteSubCommands(presentGCommandBuffer->GetGLCommandBuffer()); //执行公共缓冲区中的命令

        //结束呈现命令缓冲区录制并提交呈现命令
        SwapChain::EndPresent(*presentGLCommandBuffer);
    }
}