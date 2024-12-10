#pragma once
#include <memory>

#include "GraphicsConfig.hpp"
#include "LightGL/Runtime/Resource/GLImage.h"
#include "LightGL/Runtime/Resource/GLImageSampler.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

namespace Light
{
    class Texture2D : public TextureBase
    {
    public:
        Texture2D(int width, int height, VkFormat format, const void* data, size_t size);

        const GLImage& GetGLImage() const;
        const GLImageView& GetGLImageView() const override;
        const GLImageSampler& GetGLImageSampler() const override;

    private:
        std::unique_ptr<GLImage> glImage;
        std::unique_ptr<GLImageView> glImageView;
        std::unique_ptr<GLImageSampler> glImageSampler;
    };
}
