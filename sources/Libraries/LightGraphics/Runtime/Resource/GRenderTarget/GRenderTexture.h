﻿#pragma once
#include <memory>

#include "GRenderTarget.hpp"
#include "../GTexture/GTexture.hpp"
#include "LightGraphics/Runtime/Graphics.h"

namespace Light
{
    class GRenderTexture : public GTexture, public GRenderTarget
    {
    public:
        GRenderTexture(
            uint32_t width, uint32_t height,
            VkFormat colorFormat = Graphics::GetGraphicsConfig().presentColorFormat,
            VkFormat depthStencilFormat = Graphics::GetGraphicsConfig().presentDepthStencilFormat,
            VkSampleCountFlagBits sampleCount = Graphics::GetGraphicsConfig().presentSampleCount);

        GLImageView& GetGLImageView() const override { return *colorImageView; }
        GLImageSampler& GetGLImageSampler() const override { return *glImageSampler; }

    private:
        std::unique_ptr<GLImage> colorImage;
        std::unique_ptr<GLImageView> colorImageView;
        std::unique_ptr<GLImage> depthStencilImage;
        std::unique_ptr<GLImageView> depthStencilImageView;
        std::unique_ptr<GLImage> colorResolveImage;
        std::unique_ptr<GLImageView> colorResolveImageView;
        GLImageSampler* glImageSampler;
    };
}