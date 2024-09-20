#pragma once
#include <vulkan/vulkan_core.h>


struct GLSubpass
{
    
};

/**
 * GLFrameBuffer指定了渲染管道所用的输出缓冲区，而GLRenderPass则描述了渲染管道应如何处理它们的内容。
 * GLRenderPass可供多个管道使用，通过subpass将多个管道执行串在一起，所以GLRenderPass才是老大，而不是GLPipeline。
 * https://zhuanlan.zhihu.com/p/461097833
 */
class GLRenderPass
{
public:
    VkRenderPass renderPass = VK_NULL_HANDLE;

    GLRenderPass(
        const VkFormat& colorAttachmentFormat, const VkSampleCountFlagBits& colorAttachmentSampleCount,
        const VkFormat& depthAttachmentFormat, const VkSampleCountFlagBits& depthAttachmentSampleCount);
    GLRenderPass(const GLRenderPass&) = delete;
    ~GLRenderPass();
};
