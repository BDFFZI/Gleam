#include "ImageImporter.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <stdexcept>

using namespace Gleam;

RawImage ImageImporter::Import(const std::string& filePath, const int desiredChannel)
{
    RawImage image;

    //像素布局为从左上开始，逐行扫描
    stbi_uc* pixels = stbi_load(
        filePath.c_str(),
        &image.width, &image.height,
        &image.channel, desiredChannel
    );
    if (pixels == nullptr)
        throw std::runtime_error("加载图片数据失败!");
    if (desiredChannel != STBI_default)
        image.channel = desiredChannel;

    size_t size = static_cast<size_t>(image.width * image.height * image.channel);
    image.pixels.resize(size);
    memcpy(image.pixels.data(), pixels, size);

    stbi_image_free(pixels);

    return image;
}
