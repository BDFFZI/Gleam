#include "RenderTexture.h"

using namespace LightRuntime;

RenderTexture::RenderTexture(const int width, const int height, const VkFormat colorFormat, const VkFormat depthFormat, const VkSampleCountFlagBits sampleCount)
    : width(width), height(height)
{
    glColorImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferColor(width, height, colorFormat, sampleCount));
    glColorImageView = std::make_unique<GLImageView>(*glColorImage, VK_IMAGE_ASPECT_COLOR_BIT);

    if (depthFormat != VK_FORMAT_UNDEFINED)
    {
        glDepthStencilImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferDepth(width, height, depthFormat, sampleCount));
        glDepthStencilImageView = std::make_unique<GLImageView>(*glColorImage, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    if (sampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        glColorResolveImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferColor(width, height, colorFormat, VK_SAMPLE_COUNT_1_BIT));
        glColorResolveImageView = std::make_unique<GLImageView>(*glColorImage, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

const std::unique_ptr<GLImageView>& RenderTexture::GetGLColorImageView() const
{
    return glColorImageView;
}
const std::unique_ptr<GLImageView>& RenderTexture::GetGLDepthStencilImageView() const
{
    return glDepthStencilImageView;
}
const std::unique_ptr<GLImageView>& RenderTexture::GetGLColorResolveImageView() const
{
    return glColorResolveImageView;
}
const std::unique_ptr<GLImage>& RenderTexture::GetGLPresentImage() const
{
    return glColorResolveImage != nullptr ? glColorResolveImage : glColorImage;
}
int RenderTexture::GetWidth() const
{
    return width;
}
int RenderTexture::GetHeight() const
{
    return height;
}
