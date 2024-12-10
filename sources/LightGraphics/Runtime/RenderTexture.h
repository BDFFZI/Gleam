#pragma once
#include <memory>

#include "GraphicsConfig.hpp"
#include "LightGL/Runtime/Resource/GLImageView.h"

namespace Light
{
    class RenderTexture : public RenderTargetBase, public TextureBase
    {
    public:
        RenderTexture(
            uint32_t width, uint32_t height,
            VkFormat colorFormat = DefaultColorFormat,
            VkFormat depthStencilFormat = DefaultDepthStencilFormat,
            VkSampleCountFlagBits sampleCount = DefaultGLStateLayout.multisample.rasterizationSamples);
        RenderTexture(const RenderTexture&) = delete;

        uint32_t GetWidth() const override { return width; }
        uint32_t GetHeight() const override { return height; }
        VkSampleCountFlagBits GetSampleCount() const override { return sampleCount; }
        const VkImage& GetVKColorImage() const override;
        const VkImage* GetVkDepthStencilImage() const override;
        const VkImage* GetVkColorResolveImage() const override;
        const GLImageView& GetGLColorImageView() const override;
        const GLImageView* GetGLDepthStencilImageView() const override;
        const GLImageView* GetGLColorResolveImageView() const override;

        const GLImageView& GetGLImageView() const override { return *glColorImageView; }
        const GLImageSampler& GetGLImageSampler() const override { return *glImageSampler; }

    private:
        uint32_t width, height;
        VkSampleCountFlagBits sampleCount;
        std::unique_ptr<GLImageSampler> glImageSampler;
        std::unique_ptr<GLImage> glColorImage;
        std::unique_ptr<GLImage> glDepthStencilImage;
        std::unique_ptr<GLImage> glColorResolveImage;
        std::unique_ptr<GLImageView> glColorImageView;
        std::unique_ptr<GLImageView> glDepthStencilImageView;
        std::unique_ptr<GLImageView> glColorResolveImageView;
    };
}
