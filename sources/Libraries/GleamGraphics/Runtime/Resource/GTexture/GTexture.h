#pragma once

#include "GleamGL/Runtime/Resource/GLImageSampler.h"
#include "GleamGL/Runtime/Resource/GLImageView.h"

namespace Gleam
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