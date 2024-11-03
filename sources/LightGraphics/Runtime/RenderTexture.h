#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLImageView.h"

namespace Light
{
    class RenderTexture
    {
    public:
        RenderTexture(
            int width, int height,
            VkFormat colorFormat,
            VkFormat depthFormat = VK_FORMAT_UNDEFINED,
            VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
        RenderTexture(const RenderTexture&) = delete;

        const std::unique_ptr<GLImageView>& GetGLColorImageView() const;
        const std::unique_ptr<GLImageView>& GetGLDepthStencilImageView() const;
        const std::unique_ptr<GLImageView>& GetGLColorResolveImageView() const;
        const std::unique_ptr<GLImage>& GetGLPresentImage() const;

        int GetWidth() const;
        int GetHeight() const;

    private:
        int width;
        int height;
        std::unique_ptr<GLImage> glColorImage;
        std::unique_ptr<GLImageView> glColorImageView;
        std::unique_ptr<GLImage> glDepthStencilImage;
        std::unique_ptr<GLImageView> glDepthStencilImageView;
        std::unique_ptr<GLImage> glColorResolveImage;
        std::unique_ptr<GLImageView> glColorResolveImageView;
    };
}
