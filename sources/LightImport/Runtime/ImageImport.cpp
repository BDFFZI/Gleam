#include "ImageImport.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

namespace LightRuntime
{
    RawImage ImportPng(const std::string& filePath)
    {
        RawImage image;

        stbi_uc* pixels = stbi_load(
            filePath.c_str(),
            &image.width, &image.height,
            nullptr, STBI_rgb_alpha
        );
        if (pixels == nullptr)
            throw std::runtime_error("加载图片数据失败!");

        size_t size = static_cast<size_t>(image.width * image.height * STBI_rgb_alpha);
        image.pixels.resize(size);
        memcpy(image.pixels.data(), pixels, size);

        stbi_image_free(pixels);

        return image;
    }
}
