#include "Texture.h"

#include "GraphicsPreset.h"
#include "LightImport/Runtime/ImageImporter.h"

namespace Light
{
    Texture2D::Texture2D(const int width, const int height, const VkFormat format, const void* data, const size_t size)
    {
        image = GLImage::CreateTexture2D(width, height, format, data, size, true);
        imageView = std::make_unique<GLImageView>(*image, VK_IMAGE_ASPECT_COLOR_BIT);

        glImageView = imageView.get();
        glImageSampler = GraphicsPreset::DefaultGLImageSampler.get();
    }
    Texture2D::Texture2D(const char* fileName)
    {
        RawImage rawImage = ImageImporter::Import(fileName);
        VkFormat format;
        switch (rawImage.channel)
        {
        case STBI_grey: format = VK_FORMAT_R8_SRGB;
            break;
        case STBI_grey_alpha: format = VK_FORMAT_R8G8_SRGB;
            break;
        case STBI_rgb: format = VK_FORMAT_R8G8B8_SRGB;
            break;
        case STBI_rgb_alpha: format = VK_FORMAT_R8G8B8A8_SRGB;
            break;
        default: throw std::runtime_error("不支持的通道类型！");
        }

        image = GLImage::CreateTexture2D(
            rawImage.width, rawImage.height, format, rawImage.pixels.data(), rawImage.pixels.size(), true);
        imageView = std::make_unique<GLImageView>(*image, VK_IMAGE_ASPECT_COLOR_BIT);

        glImageView = imageView.get();
        glImageSampler = GraphicsPreset::DefaultGLImageSampler.get();
    }
}
