#pragma once
#include <memory>

#include "RenderPass.h"
#include "LightGL/Runtime/Pipeline/GLFramebuffer.h"
#include "LightGL/Runtime/Resource/GLImageView.h"

class RenderTarget
{
public:
    RenderTarget(const RenderPass& renderPass, const int width, const int height, const VkFormat colorFormat, const VkFormat depthFormat, const VkSampleCountFlagBits sampleCount,
                 const GLImageView& colorResolveView)
    {
        glColorImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferColor(width, height, colorFormat, sampleCount));
        glColorImageView = std::make_unique<GLImageView>(*glColorImage, VK_IMAGE_ASPECT_COLOR_BIT);
        glDepthImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferDepth(width, height, depthFormat, sampleCount));
        glDepthImageView = std::make_unique<GLImageView>(*glColorImage, VK_IMAGE_ASPECT_DEPTH_BIT);
        glFramebuffer = std::make_unique<GLFramebuffer>(
            renderPass.GetGLRenderPass(),
            std::vector{
                glColorImageView->imageView,
                glDepthImageView->imageView,
                colorResolveView.imageView
            },
            VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)}
        );
    }
    RenderTarget(const RenderTarget&) = delete;

    std::unique_ptr<GLImage> glColorImage;
    std::unique_ptr<GLImageView> glColorImageView;
    std::unique_ptr<GLImage> glDepthImage;
    std::unique_ptr<GLImageView> glDepthImageView;
    std::unique_ptr<GLFramebuffer> glFramebuffer;
};
