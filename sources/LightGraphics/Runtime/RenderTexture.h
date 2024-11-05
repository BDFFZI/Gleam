#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLImageView.h"

namespace Light
{
    class RenderTextureBase
    {
    public:
        virtual ~RenderTextureBase() = default;
        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;
        virtual const VkImage& GetVKColorImage() const = 0;
        virtual const VkImage* GetVkDepthStencilImage() const = 0;
        virtual const VkImage* GetVkColorResolveImage() const = 0;
        virtual const GLImageView& GetGLColorImageView() const = 0;
        virtual const GLImageView* GetGLDepthStencilImageView() const = 0;
        virtual const GLImageView* GetGLColorResolveImageView() const = 0;
    };

    class RenderTexture : public RenderTextureBase
    {
    public:
        RenderTexture(
            uint32_t width, uint32_t height,
            VkFormat colorFormat,
            VkFormat depthFormat = VK_FORMAT_UNDEFINED,
            VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
        RenderTexture(const RenderTexture&) = delete;

        uint32_t GetWidth() const override { return width; }
        uint32_t GetHeight() const override { return height; }
        const VkImage& GetVKColorImage() const override;
        const VkImage* GetVkDepthStencilImage() const override;
        const VkImage* GetVkColorResolveImage() const override;
        const GLImageView& GetGLColorImageView() const override;
        const GLImageView* GetGLDepthStencilImageView() const override;
        const GLImageView* GetGLColorResolveImageView() const override;

    private:
        uint32_t width, height;
        std::unique_ptr<GLImage> glColorImage;
        std::unique_ptr<GLImage> glDepthStencilImage;
        std::unique_ptr<GLImage> glColorResolveImage;
        std::unique_ptr<GLImageView> glColorImageView;
        std::unique_ptr<GLImageView> glDepthStencilImageView;
        std::unique_ptr<GLImageView> glColorResolveImageView;
    };
}
