#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLImage.h"
#include "LightGL/Runtime/Resource/GLImageSampler.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

class Texture2D
{
public:
    Texture2D(int width, int height, VkFormat format, const void* data, size_t size);

    const GLImage& GetGLImage() const;
    const GLImageView& GetGLImageView() const;
    const GLImageSampler& GetGLImageSampler() const;

private:
    std::unique_ptr<GLImage> glImage;
    std::unique_ptr<GLImageView> glImageView;
    std::unique_ptr<GLImageSampler> glImageSampler;
};
