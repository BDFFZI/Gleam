#include "PresentationSystem.h"
#include "Graphics/SwapChain.h"
#include "Graphics/Graphics.h"
#include "LightWindow/Runtime/Window.h"

void Light::PresentationSystem::Start()
{
    std::vector<const char*> extensions;
    Graphics::InitializeGLDemand(extensions);
    GL gl = GL::Initialize(Window->GetGlfwWindow(), extensions);
    Graphics::Initialize(gl);

    commandBuffer = &Graphics::ApplyCommandBuffer();

    SystemGroup::Start();
}
void Light::PresentationSystem::Stop()
{
    SystemGroup::Stop();

    Graphics::ReleaseCommandBuffer(*commandBuffer);

    Graphics::UnInitialize();
    GL::UnInitialize();
}
void Light::PresentationSystem::Update()
{
    //开始呈现并录制呈现命令缓冲区
    if (SwapChain::BeginPresent(&presentCommandBuffer))
    {
        //开始公共命令缓冲区录制
        commandBuffer->BeginRecording();

        SystemGroup::Update();

        //完成公共命令缓冲区录制
        commandBuffer->EndRecording();
        //执行公共缓冲区中的命令
        presentCommandBuffer->ExecuteSubCommands(commandBuffer->GetGLCommandBuffer());
        //结束呈现命令缓冲区录制并提交呈现命令
        SwapChain::EndPresent(*presentCommandBuffer);
        //等待呈现完成
        SwapChain::WaitPresent();
    }
}