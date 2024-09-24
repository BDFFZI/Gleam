#include "GLRenderPass.h"

#include <stdexcept>

#include "../Foundation/GLFoundation.h"

VkAttachmentDescription CreateAttachmentDescription(const GLAttachmentInfo& attachment)
{
    VkAttachmentDescription attachmentDescription = {};
    attachmentDescription.format = attachment.format;
    attachmentDescription.samples = attachment.samples;

    switch (attachment.type)
    {
    case GLAttachmentType::Color:
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //执行前清除颜色数据为指定常数
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //执行后保持颜色数据
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //不定义执行前模板数据行为
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //不定义执行后模板数据行为
    //vk中使用纹理时必须将其调整为兼容下一步操作的内存布局
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //执行前纹理应具有的内存布局（选择未定义则会导致无法保持原本的图像内容，但正好不需要，因为使用前会被清除）
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //选择颜色缓冲区的最优布局即可
        break;
    case GLAttachmentType::DepthStencil:
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //执行前清除深度数据为指定常数
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //执行后不使用深度数据
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //不定义执行前模板数据行为
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //不定义执行后模板数据行为
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //不关心执行前纹理布局，因为其内容执行前会被清除
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //选择深度缓冲区的最优布局即可
        break;
    case GLAttachmentType::ColorResolve:
        attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //最终布局应适用于呈现
        break;
    }

    return attachmentDescription;
}

GLRenderPass::GLRenderPass(const std::vector<GLAttachmentInfo>& glAttachmentInfos,
                           const std::vector<GLSubpass>& glSubpasses,
                           const std::vector<VkSubpassDependency>& glSubpassDependencies)
{
    //定义Pass中所用附件（输出缓冲区）信息，以及其在管道开始和结束时的处理（中间过程的处理由Subpass描述）
    std::vector<VkAttachmentDescription> attachments(glAttachmentInfos.size());
    for (size_t i = 0; i < glAttachmentInfos.size(); i++)
        attachments[i] = CreateAttachmentDescription(glAttachmentInfos[i]);

    //Pass由一个或多个Subpass构成，一次Subpass代表一次管道运行，同时会继承上次Subpass的渲染结果，Subpass也是对输出缓冲区的处理过程，故需要描述处理方式
    //vulkan允许多个管道同时运行，并且是Subpass决定了所用管道，所以该subpass与其他游戏引擎中的subpass不完全一样
    std::vector<VkSubpassDescription> subpasses(glSubpasses.size());
    for (size_t i = 0; i < glSubpasses.size(); i++)
    {
        const auto& [
            colorAttachments,
            depthStencilAttachment,
            colorResolveAttachment
        ] = glSubpasses[i];

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; //该pass图像管道上执行
        subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size()); //不使用“多渲染目标”功能，故只输出一个颜色附件
        subpass.pColorAttachments = colorAttachments.data(); //输出的颜色附件引用
        subpass.pDepthStencilAttachment = depthStencilAttachment.has_value() ? &depthStencilAttachment.value() : nullptr; //输出的深度模板附件引用
        subpass.pResolveAttachments = colorResolveAttachment.has_value() ? &colorResolveAttachment.value() : nullptr; //启用多重采样功能时的颜色解析附件
        subpasses[i] = subpass;
    }

    //创建最终的renderPass信息
    VkRenderPassCreateInfo renderPassInfo{};
    {
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
        renderPassInfo.pSubpasses = subpasses.data();
        renderPassInfo.dependencyCount = static_cast<uint32_t>(glSubpassDependencies.size());
        renderPassInfo.pDependencies = glSubpassDependencies.data();
    }

    if (vkCreateRenderPass(GLFoundation::glDevice->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        throw std::runtime_error("无法创建渲染过程!");
}
GLRenderPass::~GLRenderPass()
{
    vkDestroyRenderPass(GLFoundation::glDevice->device, renderPass, nullptr);
}
