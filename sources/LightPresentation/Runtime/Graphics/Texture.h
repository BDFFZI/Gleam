#pragma once
#include <memory>

#include "GraphicsAssets.h"
#include "../GL/Resource/GLImage.h"
#include "../GL/Resource/GLImageView.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    class Texture2D : public TextureAsset
    {
    public:
        Texture2D(int width, int height, VkFormat format, const void* data, size_t size, bool mipChain = false);
        Texture2D(int width, int height, VkFormat format, std::initializer_list<std::byte> data, bool mipChain = false);
        Texture2D(float4 color);
        Texture2D(const char* fileName);

    private:
        std::unique_ptr<GLImage> image;
        std::unique_ptr<GLImageView> imageView;
    };
}