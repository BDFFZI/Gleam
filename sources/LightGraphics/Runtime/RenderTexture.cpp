#include "RenderTexture.h"

using namespace Light;


RenderTexture::RenderTexture(
    const uint32_t width, const uint32_t height,
    const VkFormat colorFormat, const VkFormat depthStencilFormat, const VkSampleCountFlagBits sampleCount)
    : width(width), height(height), sampleCount(sampleCount)
{
    glImageSampler = std::make_unique<GLImageSampler>();
    glColorImage = GLImage::CreateFrameBufferColor(width, height, colorFormat, sampleCount);
    glColorImageView = std::make_unique<GLImageView>(*glColorImage, VK_IMAGE_ASPECT_COLOR_BIT);

    if (depthStencilFormat != VK_FORMAT_UNDEFINED)
    {
        glDepthStencilImage = GLImage::CreateFrameBufferDepth(width, height, depthStencilFormat, sampleCount);
        glDepthStencilImageView = std::make_unique<GLImageView>(*glDepthStencilImage, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    if (sampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        glColorResolveImage = GLImage::CreateFrameBufferColor(width, height, colorFormat, VK_SAMPLE_COUNT_1_BIT);
        glColorResolveImageView = std::make_unique<GLImageView>(*glColorResolveImage, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

// const VkImage& RenderTexture::GetVKColorImage() const
// {
//     return glColorImage->image;
// }
// const VkImage* RenderTexture::GetVkDepthStencilImage() const
// {
//     return glDepthStencilImage == nullptr ? nullptr : &glDepthStencilImage->image;
// }
// const VkImage* RenderTexture::GetVkColorResolveImage() const
// {
//     return glColorResolveImage == nullptr ? nullptr : &glColorResolveImage->image;
// }
// const GLImageView& RenderTexture::GetGLColorImageView() const
// {
//     return *glColorImageView;
// }
// const GLImageView* RenderTexture::GetGLDepthStencilImageView() const
// {
//     return glDepthStencilImageView.get();
// }
// const GLImageView* RenderTexture::GetGLColorResolveImageView() const
// {
//     return glColorResolveImageView.get();
// }
