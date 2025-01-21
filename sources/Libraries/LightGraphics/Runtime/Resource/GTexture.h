#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLImage.h"
#include "LightGL/Runtime/Resource/GLImageView.h"
#include "LightMath/Runtime/Vector.hpp"

class GLImageSampler;

namespace Light
{
    class GTexture
    {
    public:
        virtual ~GTexture() = default;
        virtual GLImageView& GetGLImageView() =0;
        virtual GLImageSampler& GetGLImageSampler() =0;
    };

    class GTexture2D : public GTexture
    {
    public:
        GTexture2D(int width, int height, VkFormat format, const void* data, size_t size, bool mipChain = false);
        GTexture2D(int width, int height, VkFormat format, std::initializer_list<std::byte> data, bool mipChain = false);
        GTexture2D(float4 color);
        GTexture2D(const char* fileName);

        GLImageView& GetGLImageView() override { return *imageView; }
        GLImageSampler& GetGLImageSampler() override { return *glImageSampler; }

    private:
        std::unique_ptr<GLImage> image;
        std::unique_ptr<GLImageView> imageView;
        GLImageSampler* glImageSampler;
    };
}