#include "GLFramebuffer.h"

#include <array>
#include <stdexcept>

#include "../Foundation/GLFoundation.h"

GLFramebuffer::GLFramebuffer(
    const GLRenderPass& glRenderPass,
    const GLImageView& colorAttachment, const GLImageView& depthAttachment, const GLImageView& colorResolveAttachment,
    const VkExtent2D extent)
    : colorFormat(colorAttachment.format), depthFormat(depthAttachment.format), extent(extent)
{
    std::array attachments = {colorAttachment.imageView, depthAttachment.imageView, colorResolveAttachment.imageView};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = glRenderPass.renderPass; //哪个渲染过程所用的帧缓冲区
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data(); //缓冲区绑定的图片视图
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(GLFoundation::glDevice->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
        throw std::runtime_error("帧缓冲区创建失败!");
}
GLFramebuffer::~GLFramebuffer()
{
    vkDestroyFramebuffer(GLFoundation::glDevice->device, framebuffer, nullptr);
}
