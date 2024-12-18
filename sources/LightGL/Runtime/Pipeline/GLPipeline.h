#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GLMeshLayout.h"
#include "GLRenderPass.h"
#include "GLShader.h"
#include "GLPipelineLayout.h"

struct GLStateLayout
{
    std::vector<VkDynamicState> dynamicStates;
    VkPipelineViewportStateCreateInfo viewport;
    VkPipelineMultisampleStateCreateInfo multisample;
    VkPipelineRasterizationStateCreateInfo rasterization;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;

    GLStateLayout()
    {
        //视口和裁剪阶段状态（为了支持窗口大小实时调整，已设置这些状态为动态状态，故在渲染时设置）
        viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport.pNext = VK_NULL_HANDLE;
        viewport.flags = 0;
        viewport.viewportCount = 0;
        viewport.scissorCount = 0;
        viewport.pViewports = VK_NULL_HANDLE; //视口信息（将着色器输出到图像的映射范围）
        viewport.pScissors = VK_NULL_HANDLE; //裁剪信息（裁剪掉图像在裁剪范围外的像素）
        //多重采样阶段状态（MSAA，常用于实现抗锯齿）
        multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample.pNext = VK_NULL_HANDLE;
        multisample.flags = 0;
        multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisample.sampleShadingEnable = VK_TRUE; //着色器多重采样（对每一次着色器输出画面也进行抗锯齿）
        multisample.minSampleShading = 0.2f;
        multisample.pSampleMask = nullptr; // Optional
        multisample.alphaToCoverageEnable = VK_FALSE; // Optional
        multisample.alphaToOneEnable = VK_FALSE; // Optional
        //光栅化阶段状态
        rasterization.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization.pNext = VK_NULL_HANDLE;
        rasterization.flags = 0;
        rasterization.depthClampEnable = VK_FALSE; //是否将深度钳制在0-1而不是剔除超出部分。
        rasterization.rasterizerDiscardEnable = VK_FALSE; //是否在栅格化阶段之前丢弃基元
        rasterization.polygonMode = VK_POLYGON_MODE_FILL; //多边形需要填充
        rasterization.cullMode = VK_CULL_MODE_BACK_BIT; //不需要背面剔除
        rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE; //多边形正反面判断按顺时针顶点方向
        rasterization.depthBiasEnable = VK_FALSE; //关闭深度偏移
        rasterization.depthBiasConstantFactor = 0.0f; //以常值进行深度偏移
        rasterization.depthBiasSlopeFactor = 0.0f; //以多边形相对相机z轴斜率进行深度偏移
        rasterization.depthBiasClamp = 0.0f; //深度偏移的最大范围
        rasterization.lineWidth = 1.0f; //基元为线时的宽度
        //深度模板测试阶段状态
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.pNext = VK_NULL_HANDLE;
        depthStencil.flags = 0;
        depthStencil.depthTestEnable = VK_TRUE; //启用深度测试
        depthStencil.depthWriteEnable = VK_TRUE; //启用深度写入
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; //深度测试通过条件为小于或等于(这样当深度为1时也可以渲染，而不是别扭的设置成0.99)
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // 最小深度
        depthStencil.maxDepthBounds = 1.0f; // 最大深度
        depthStencil.stencilTestEnable = VK_FALSE; //不使用模板测试
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional
        //缓冲区混合阶段状态
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA; //输出颜色系数
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA; //缓冲区颜色系数
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; //颜色混合方式，一般都为相加
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //输出透明度系数
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //缓冲区透明度系数
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; //透明度混合方式
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    }

    void SetViewportCount(const uint32_t viewportCount, const uint32_t scissorCount)
    {
        viewport.viewportCount = viewportCount;
        viewport.scissorCount = scissorCount;
    }
    void SetViewport(const std::vector<VkViewport>* viewports, const std::vector<VkRect2D>* scissors)
    {
        viewport.viewportCount = static_cast<uint32_t>(viewports->size());
        viewport.pViewports = viewports->data();
        viewport.scissorCount = static_cast<uint32_t>(scissors->size());
        viewport.pScissors = scissors->data();
    }
    void SetRasterizationSamples(const VkSampleCountFlagBits rasterizationSamples)
    {
        multisample.rasterizationSamples = rasterizationSamples;
    }
    void SetLineWidth(const float lineWidth)
    {
        rasterization.lineWidth = lineWidth;
    }
    void SetCull(const VkCullModeFlags cullMode)
    {
        rasterization.cullMode = cullMode;
    }
    void SetOffset(const bool depthBiasEnable, const float depthBiasSlopeFactor = 0, const float depthBiasConstantFactor = 0)
    {
        rasterization.depthBiasClamp = depthBiasEnable;
        rasterization.depthBiasSlopeFactor = depthBiasSlopeFactor;
        rasterization.depthBiasConstantFactor = depthBiasConstantFactor;
    }
    void SetZClip(const bool depthClampEnable)
    {
        rasterization.depthClampEnable = depthClampEnable;
    }
    void SetZTest(const VkCompareOp depthCompareOp)
    {
        depthStencil.depthCompareOp = depthCompareOp;
    }
    void SetZWrite(const bool depthWriteEnable)
    {
        depthStencil.depthWriteEnable = depthWriteEnable;
    }
    void SetBlend(const bool blendEnable, const VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA, const VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)
    {
        colorBlendAttachment.blendEnable = blendEnable;
        colorBlendAttachment.srcColorBlendFactor = srcColorBlendFactor; //输出颜色系数
        colorBlendAttachment.dstColorBlendFactor = dstColorBlendFactor; //缓冲区颜色系数
    }
    void SetBlendOp(const VkBlendOp colorBlendOp)
    {
        colorBlendAttachment.colorBlendOp = colorBlendOp;
    }
    void SetColorMask(const VkColorComponentFlags colorMask)
    {
        colorBlendAttachment.colorWriteMask = colorMask;
    }

private:
};

class GLPipeline
{
public:
    VkPipeline pipeline;

    GLPipeline(
        const GLRenderPass& glRenderPass, int subpassIndex,
        const std::vector<GLShader>& glShaderLayout,
        const GLMeshLayout& glMeshLayout,
        const GLPipelineLayout& glPipelineLayout,
        const GLStateLayout& glStateLayout = {});
    /**
     * @brief 创建适用于动态渲染的管线
     * @param colorAttachments 
     * @param depthStencilAttachment 
     * @param glShaderLayout 
     * @param glMeshLayout 
     * @param glPipelineLayout 
     * @param glStateLayout 
     */
    GLPipeline(
        const std::vector<VkFormat>& colorAttachments,
        VkFormat depthStencilAttachment,
        const std::vector<GLShader>& glShaderLayout,
        const GLMeshLayout& glMeshLayout,
        const GLPipelineLayout& glPipelineLayout,
        const GLStateLayout& glStateLayout = {});
    GLPipeline(const GLPipeline&) = delete;
    ~GLPipeline();
};
