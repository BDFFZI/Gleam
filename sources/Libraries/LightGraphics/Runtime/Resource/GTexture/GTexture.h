#pragma once

#include "LightGL/Runtime/Resource/GLImageSampler.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

namespace Light
{
    class GTexture
    {
    public:
        virtual ~GTexture() = default;
        virtual GLImage& GetGLImage() const = 0;
        virtual GLImageView& GetGLImageView() const =0;
        virtual GLImageSampler& GetGLImageSampler() const =0;
    };
}