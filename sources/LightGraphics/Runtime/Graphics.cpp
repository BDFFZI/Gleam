#include "Graphics.h"
#include "SwapChain.h"

using namespace Light;

void Graphics::InitializeGLDemand(std::vector<const char*>& extensions)
{
    extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME); //为了支持推送描述符，简化描述符集创建
}

Graphics Graphics::Initialize(
    GL&, const VkFormat presentColorFormat, const VkFormat presentDepthStencilFormat, const VkSampleCountFlagBits presentSampleCount)
{
    paintCommandBufferPool = std::make_unique<ObjectPool<CommandBuffer>>();
    SwapChain::Initialize(presentColorFormat, presentDepthStencilFormat, presentSampleCount);
    return {};
}
void Graphics::UnInitialize()
{
    vkDeviceWaitIdle(GL::glDevice->device);
    paintCommandBufferPool.reset();
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
