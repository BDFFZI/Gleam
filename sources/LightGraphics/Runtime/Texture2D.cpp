#include "Texture2D.h"

Texture2D::Texture2D(const int width, const int height, const VkFormat format, const void* data, const size_t size)
{
    glImage = std::unique_ptr<GLImage>(GLImage::CreateTexture2D(width, height, format, data, size, true));
    glImageView = std::make_unique<GLImageView>(*glImage, VK_IMAGE_ASPECT_COLOR_BIT);
    glImageSampler = std::make_unique<GLImageSampler>();
}

const GLImage& Texture2D::GetGLImage() const
{
    return *glImage;
}
const GLImageView& Texture2D::GetGLImageView() const
{
    return *glImageView;
}
const GLImageSampler& Texture2D::GetGLImageSampler() const
{
    return *glImageSampler;
}
