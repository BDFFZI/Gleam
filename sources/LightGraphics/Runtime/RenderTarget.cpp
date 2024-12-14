#include "RenderTarget.h"

using namespace Light;

RenderTexture::RenderTexture(
    const uint32_t width, const uint32_t height, // NOLINT
    const VkFormat colorFormat, const VkFormat depthStencilFormat, const VkSampleCountFlagBits sampleCount)
{
    colorImage = GLImage::CreateFrameBufferColor(width, height, colorFormat, sampleCount);
    colorImageView = std::make_unique<GLImageView>(*colorImage, VK_IMAGE_ASPECT_COLOR_BIT);
    if (depthStencilFormat != VK_FORMAT_UNDEFINED)
    {
        depthStencilImage = GLImage::CreateFrameBufferDepth(width, height, depthStencilFormat, sampleCount);
        depthStencilImageView = std::make_unique<GLImageView>(*depthStencilImage, VK_IMAGE_ASPECT_DEPTH_BIT);
    }
    if (sampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        colorResolveImage = GLImage::CreateFrameBufferColor(width, height, colorFormat, VK_SAMPLE_COUNT_1_BIT);
        colorResolveImageView = std::make_unique<GLImageView>(*colorResolveImage, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    glFinalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    glFinalImage = colorResolveImage ? colorResolveImage->image : colorImage->image;
    RenderTargetAsset::width = width;
    RenderTargetAsset::height = height;
    glColorImageView = colorImageView.get();
    glDepthStencilImageView = depthStencilImageView.get();
    glColorResolveImageView = colorResolveImageView.get();

    glImageView = colorImageView.get();
    glImageSampler = GraphicsPreset::DefaultGLImageSampler.get();
}
