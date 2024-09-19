#include "GLRenderPass.h"

#include <array>
#include <stdexcept>

#include "../Foundation/GLFoundation.h"

GLRenderPass::GLRenderPass(const VkFormat& colorAttachmentFormat, const VkFormat& depthAttachmentFormat)
{
    //定义Pass中所用附件（输出缓冲区）的处理方式，主要是管道开始和结束时的处理，因为中间的处理过程需要用Subpass描述
    VkAttachmentDescription colorAttachment{};
    VkAttachmentDescription depthAttachment{};
    {
        colorAttachment.format = colorAttachmentFormat; //颜色纹理格式
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //不使用多重采样
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //执行前清除颜色数据为指定常数
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; //执行后保持颜色数据
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //不定义执行前模板数据行为
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //不定义执行后模板数据行为
        //vk中使用纹理时必须将其调整为兼容下一步操作的内存布局
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //执行前纹理应具有的内存布局（选择未定义则会导致无法保持原本的图像内容，但正好不需要，因为使用前会被清除）
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //执行后纹理内存布局应过渡到适用于呈现的布局

        depthAttachment.format = depthAttachmentFormat; //深度模板纹理格式
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //不使用多重采样
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; //执行前清除深度数据为指定常数
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //执行后不使用深度数据
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; //不定义执行前模板数据行为
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; //不定义执行后模板数据行为
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //不关心执行前纹理布局，因为其内容执行前会被清除
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //???
    }

    //Pass由一个或多个Subpass构成，一次Subpass代表一次管道运行，同时会继承上次Subpass的渲染结果，Subpass也是对输出缓冲区的处理过程，故需要描述处理方式
    //vulkan允许多个管道同时运行，并且是Subpass决定了所用管道，所以该subpass与其他游戏引擎中的subpass不完全一样
    VkSubpassDescription subpass{};
    {
        VkAttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0; //使用第1个附件（颜色缓冲区）
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; //当执行该subpass时纹理应过渡到的布局，此时需要做正常的颜色缓冲区使用
        VkAttachmentReference depthAttachmentRef;
        depthAttachmentRef.attachment = 1; //使用第2个附件（深度缓冲区）
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; //此时该纹理需做深度模板缓冲区使用

        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; //该pass图像管道上执行
        subpass.colorAttachmentCount = 1; //不使用“多渲染目标”功能，故只输出一个颜色附件
        subpass.pColorAttachments = &colorAttachmentRef; //输出的颜色附件引用
        subpass.pDepthStencilAttachment = &depthAttachmentRef; //输出的深度模板附件引用
    }


    //pass中图像的内存布局变化是由两个内置的隐式“subpass”实现，默认在pass管线开始和结束时执行。
    //由于使用的异步渲染，故在颜色输出阶段前，颜色附件不一定可用，故需要调整内置开始subpass的执行时机。
    //通过subpass依赖功能调整执行时机，subpass依赖需要同时描述两个相邻subpass信息，借此实现subpass执行顺序的确定。
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL; //在srcSubPass中定义该值表示前置隐式subpass（放dstSubpass中表示后置）
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT; //等待到颜色输出和片段测试（深度测试）阶段开始时执行subpass处理
    dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //此阶段需要清除深度纹理（但颜色为什么不要？？？）
    dependency.dstSubpass = 0; //表明srcSubpass的后一个执行的subpass，此处即我们的subpass，通过srcSubpass+dstSubpass便实现了subpass执行顺序的确定
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; //等待到颜色输出和片段测试（深度测试）阶段开始时执行subpass处理
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; //执行期间需写入颜色和深度信息

    //创建最终的renderPass信息
    VkRenderPassCreateInfo renderPassInfo{};
    {
        const std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
    }

    if (vkCreateRenderPass(GLFoundation::glDevice->device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        throw std::runtime_error("无法创建渲染过程!");
}
GLRenderPass::~GLRenderPass()
{
    vkDestroyRenderPass(GLFoundation::glDevice->device, renderPass, nullptr);
}
