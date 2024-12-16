#include "Graphics.h"
#include "SwapChain.h"

using namespace Light;

void Graphics::InitializeGLDemand(std::vector<const char*>& extensions)
{
    extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME); //为了支持推送描述符，简化描述符集创建
}

Graphics Graphics::Initialize(GL&)
{
    GraphicsPreset::Initialize();
    SwapChain::Initialize(
        GraphicsPreset::DefaultColorFormat,
        GraphicsPreset::DefaultDepthStencilFormat,
        GraphicsPreset::DefaultStateLayout.multisample.rasterizationSamples);
    paintCommandBufferPool = std::make_unique<ObjectPool<CommandBuffer>>();
    defaultRenderTarget = &SwapChain::GetPresentRenderTarget();
    return {};
}
void Graphics::UnInitialize()
{
    vkDeviceWaitIdle(GL::glDevice->device);
    paintCommandBufferPool.reset();
    SwapChain::UnInitialize();
    GraphicsPreset::UnInitialize();
}

CommandBuffer& Graphics::ApplyCommandBuffer(const std::string& name)
{
    return paintCommandBufferPool->Get();
}
void Graphics::ReleaseCommandBuffer(CommandBuffer& commandBuffer)
{
    paintCommandBufferPool->Release(commandBuffer);
}

RenderTargetAsset& Graphics::GetDefaultRenderTarget()
{
    return *defaultRenderTarget;
}
void Graphics::SetDefaultRenderTarget(RenderTargetAsset& renderTarget)
{
    defaultRenderTarget = &renderTarget;
}
