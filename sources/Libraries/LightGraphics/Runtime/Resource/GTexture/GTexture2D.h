#pragma once
#include "GTexture.hpp"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    class GTexture2D : public GTexture
    {
    public:
        GTexture2D(int width, int height, VkFormat format, const void* data, size_t size, bool mipChain = false);
        GTexture2D(int width, int height, VkFormat format, std::initializer_list<std::byte> data, bool mipChain = false);
        GTexture2D(float4 color);
        GTexture2D(const char* fileName);

        GLImageView& GetGLImageView() const override { return *imageView; }
        GLImageSampler& GetGLImageSampler() const override { return *glImageSampler; }

    private:
        std::unique_ptr<GLImage> image;
        std::unique_ptr<GLImageView> imageView;
        GLImageSampler* glImageSampler;
    };
}