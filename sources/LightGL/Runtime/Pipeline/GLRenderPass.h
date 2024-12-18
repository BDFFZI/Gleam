#pragma once
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>


enum class GLAttachmentType:std::uint8_t
{
    Color,
    DepthStencil,
    ColorResolve,
};

struct GLAttachmentInfo
{
    GLAttachmentType type;
    VkFormat format;
    VkSampleCountFlagBits samples;
};

struct GLSubpass
{
    std::vector<VkAttachmentReference> colorAttachments;
    std::optional<VkAttachmentReference> depthStencilAttachment;
    std::optional<VkAttachmentReference> colorResolveAttachment;
};

/**
 * GLFrameBuffer指定了渲染管道所用的输出缓冲区，而GLRenderPass则描述了渲染管道应如何处理它们的内容。
 * GLRenderPass可供多个管道使用，通过subpass将多个管道执行串在一起，所以GLRenderPass才是老大，而不是GLPipeline。
 * https://zhuanlan.zhihu.com/p/461097833
 * https://zhuanlan.zhihu.com/p/619295431
 */
class GLRenderPass
{
public:
    VkRenderPass renderPass = VK_NULL_HANDLE;

    GLRenderPass(
        const std::vector<GLAttachmentInfo>& glAttachmentInfos,
        const std::vector<GLSubpass>& glSubpasses,
        const std::vector<VkSubpassDependency>& glSubpassDependencies);
    GLRenderPass(const GLRenderPass&) = delete;
    ~GLRenderPass();
};
