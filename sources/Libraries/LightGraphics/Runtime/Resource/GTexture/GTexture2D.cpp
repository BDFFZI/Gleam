#include "GTexture2D.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightImport/Runtime/ImageImporter.h"

namespace Light
{
    GTexture2D::GTexture2D(const int width, const int height, const VkFormat format,
                           const void* data, const size_t size, const bool mipChain)
    {
        image = GLImage::CreateTexture2D(width, height, format, data, size, mipChain);
        imageView = std::make_unique<GLImageView>(*image, VK_IMAGE_ASPECT_COLOR_BIT);
        glImageSampler = &Graphics::GetDefaultGLImageSampler();
    }
    GTexture2D::GTexture2D(const int width, const int height, const VkFormat format,
                           const std::initializer_list<std::byte> data, const bool mipChain)
        : GTexture2D(width, height, format, data.begin(), data.size(), mipChain)
    {
    }
    GTexture2D::GTexture2D(const float4 color)
        : GTexture2D(1, 1, VK_FORMAT_R8G8B8A8_SRGB,
                     std::initializer_list{
                         static_cast<uint8_t>(color.x * 255),
                         static_cast<uint8_t>(color.y * 255),
                         static_cast<uint8_t>(color.z * 255),
                         static_cast<uint8_t>(color.w * 255)
                     }.begin(), 4)
    {
    }
    GTexture2D::GTexture2D(const char* fileName)
    {
        RawImage rawImage = ImageImporter::Import(fileName, STBI_rgb_alpha);
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

        new(this) GTexture2D(
            rawImage.width, rawImage.height, format,
            rawImage.pixels.data(), rawImage.pixels.size(), true);
    }
}