#include "GLFramebuffer.h"

#include <stdexcept>

#include "../GL.h"

GLFramebuffer::GLFramebuffer(
    const GLRenderPass& glRenderPass,
    const std::vector<VkImageView>& attachments,
    const VkExtent2D extent)
    : extent(extent)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = glRenderPass.renderPass; //哪个渲染过程所用的帧缓冲区
    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments = attachments.data(); //缓冲区绑定的图片视图
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(GL::glDevice->device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
        throw std::runtime_error("帧缓冲区创建失败!");
}
GLFramebuffer::~GLFramebuffer()
{
    vkDestroyFramebuffer(GL::glDevice->device, framebuffer, nullptr);
}
