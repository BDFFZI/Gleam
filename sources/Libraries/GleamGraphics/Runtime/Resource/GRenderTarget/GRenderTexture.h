#pragma once
#include <memory>

#include "GRenderTarget.h"
#include "../GTexture/GTexture.h"
#include "GleamGraphics/Runtime/Graphics.h"

namespace Gleam
{
    class GRenderTexture : public GTexture, public GRenderTarget
    {
    public:
        GRenderTexture(
            uint32_t width, uint32_t height,
            VkFormat colorFormat = Graphics::GetGraphicsConfig().presentColorFormat,
            VkFormat depthStencilFormat = Graphics::GetGraphicsConfig().presentDepthStencilFormat,
            VkSampleCountFlagBits sampleCount = Graphics::GetGraphicsConfig().presentSampleCount);
        GRenderTexture(
            int2 size,
            VkFormat colorFormat = Graphics::GetGraphicsConfig().presentColorFormat,
            VkFormat depthStencilFormat = Graphics::GetGraphicsConfig().presentDepthStencilFormat,
            VkSampleCountFlagBits sampleCount = Graphics::GetGraphicsConfig().presentSampleCount);

        GLImage& GetGLImage() const override;
        GLImageView& GetGLImageView() const override;
        GLImageSampler& GetGLImageSampler() const override;
        void SetGLFinalLayout(VkImageLayout imageLayout);

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