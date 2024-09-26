#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLImage.h"
#include "LightGL/Runtime/Resource/GLImageSampler.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

class Texture2D
{
public:
    Texture2D(const int width, const int height, const void* data, const int size)
    {
        glImage = std::unique_ptr<GLImage>(GLImage::CreateTexture2D(width, height, data, size, true));
        glImageView = std::make_unique<GLImageView>(*glImage, VK_IMAGE_ASPECT_COLOR_BIT);
        glImageSampler = std::make_unique<GLImageSampler>();
    }

    const GLImage& GetGLImage() const { return *glImage; }
    const GLImageView& GetGLImageView() const { return *glImageView; }
    const GLImageSampler& GetGLImageSampler() const { return *glImageSampler; }

private:
    std::unique_ptr<GLImage> glImage;
    std::unique_ptr<GLImageView> glImageView;
    std::unique_ptr<GLImageSampler> glImageSampler;
};
