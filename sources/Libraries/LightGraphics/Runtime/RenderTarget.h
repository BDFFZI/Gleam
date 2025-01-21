#pragma once
#include <memory>

#include "GraphicsPreset.hpp"
#include "Texture.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

namespace Light
{
    class RenderTexture : public RenderTargetAsset, public TextureAsset
    {
    public:
        RenderTexture(
            uint32_t width, uint32_t height,
            VkFormat colorFormat = GraphicsPreset::DefaultColorFormat,
            VkFormat depthStencilFormat = GraphicsPreset::DefaultDepthStencilFormat,
            VkSampleCountFlagBits sampleCount = GraphicsPreset::DefaultStateLayout.multisample.rasterizationSamples);
        RenderTexture(const RenderTexture&) = delete;

    private:
        std::unique_ptr<GLImage> colorImage;
        std::unique_ptr<GLImageView> colorImageView;
        std::unique_ptr<GLImage> depthStencilImage;
        std::unique_ptr<GLImageView> depthStencilImageView;
        std::unique_ptr<GLImage> colorResolveImage;
        std::unique_ptr<GLImageView> colorResolveImageView;
    };
}
