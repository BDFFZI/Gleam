#include "Graphics.h"
#include "SwapChain.h"
#include "Resource/GTexture.h"

using namespace Light;

void Graphics::InitializeGLDemand(std::vector<const char*>& extensions)
{
    extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME); //为了支持推送描述符，简化描述符集创建
}
Graphics Graphics::Initialize(GL&, const GraphicsPreset& defaultPreset)
{
    paintCommandBufferPool = std::make_unique<ObjectPool<CommandBuffer>>();
    defaultGraphicsPreset = defaultPreset;
    defaultGLDescriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(defaultPreset.descriptorBindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR);
    defaultGLPipelineLayout = std::make_unique<GLPipelineLayout>(*defaultGLDescriptorSetLayout, PushConstantRanges);
    defaultGLImageSampler = std::make_unique<GLImageSampler>();
    defaultTexture2D = std::make_unique<GTexture2D>(1);
    SwapChain::Initialize(
        defaultPreset.colorFormat,
        defaultPreset.depthStencilFormat,
        defaultPreset.stateLayout.multisample.rasterizationSamples);
    defaultRenderTarget = &SwapChain::GetPresentRenderTarget();

    return {};
}
void Graphics::UnInitialize()
{
    vkDeviceWaitIdle(GL::glDevice->device);
    paintCommandBufferPool.reset();
    
    defaultGLDescriptorSetLayout.reset();
    defaultGLPipelineLayout.reset();
    defaultGLImageSampler.reset();
    defaultTexture2D.reset();
    
    SwapChain::UnInitialize();
}

CommandBuffer& Graphics::ApplyCommandBuffer(const std::string& name)
{
    return paintCommandBufferPool->Get();
}
void Graphics::ReleaseCommandBuffer(CommandBuffer& commandBuffer)
{
    paintCommandBufferPool->Release(commandBuffer);
}