#include "Graphics.h"
#include "SwapChain.h"

using namespace Light;

void Graphics::InitializeGLDemand(std::vector<const char*>& extensions)
{
    extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME); //为了支持推送描述符，简化描述符集创建
}
Graphics Graphics::Initialize(GL&, std::unique_ptr<GraphicsConfig>&& defaultPreset)
{
    graphicsConfig = std::move(defaultPreset);
    defaultGLImageSampler = std::make_unique<GLImageSampler>();
    defaultTexture2D = std::make_unique<GTexture2D>(1.0f);
    SwapChain::Initialize(
        graphicsConfig->presentColorFormat,
        graphicsConfig->presentDepthStencilFormat,
        graphicsConfig->presentSampleCount);

    return {};
}
void Graphics::UnInitialize()
{
    vkDeviceWaitIdle(GL::glDevice->device);

    graphicsConfig.reset();
    defaultGLImageSampler.reset();
    defaultTexture2D.reset();

    SwapChain::UnInitialize();
}