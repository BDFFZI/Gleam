#pragma once
#include <vulkan/vulkan_core.h>
#include "Pipeline/GLRenderPass.h"

/**
 * 存储了渲染管道中的最终输出缓冲区对应的实际物体，而关于这些缓冲区的描述则由GLRenderPass描述。
 * 渲染管道可以有多个输出缓冲区，这些缓冲区又被称作配件，一般都采用一个颜色缓冲区加深度缓冲区的方式。
 */
class GLFramebuffer
{
public:
    VkFramebuffer framebuffer;
    VkExtent2D extent;

    GLFramebuffer(const GLRenderPass& glRenderPass, const std::vector<VkImageView>& attachments, VkExtent2D extent);
    GLFramebuffer(const GLFramebuffer&) = delete;
    ~GLFramebuffer();
};
