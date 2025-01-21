#include "Graphics.h"
#include "SwapChain.h"

using namespace Light;

void Graphics::InitializeGLDemand(std::vector<const char*>& extensions)
{
    extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME); //为了支持推送描述符，简化描述符集创建
}
Graphics Graphics::Initialize(GL&, const GraphicsPreset& defaultPreset)
{
    defaultGraphicsPreset = defaultPreset;
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
    
    defaultGLImageSampler.reset();
    defaultTexture2D.reset();

    SwapChain::UnInitialize();
}