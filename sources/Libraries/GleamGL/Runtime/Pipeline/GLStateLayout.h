#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

class GLStateLayout
{
public:
    std::vector<VkDynamicState> dynamicStates;
    VkPipelineViewportStateCreateInfo viewport;
    VkPipelineMultisampleStateCreateInfo multisample;
    VkPipelineRasterizationStateCreateInfo rasterization;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;

    GLStateLayout();

    void SetViewportCount(uint32_t viewportCount, uint32_t scissorCount);
    void SetViewport(const std::vector<VkViewport>* viewports, const std::vector<VkRect2D>* scissors);
    void SetRasterizationSamples(VkSampleCountFlagBits rasterizationSamples);
    void SetLineWidth(float lineWidth);
    void SetCull(VkCullModeFlags cullMode);
    void SetOffset(bool depthBiasEnable, float depthBiasSlopeFactor = 0, float depthBiasConstantFactor = 0);
    void SetZClip(bool depthClampEnable);
    void SetZTest(VkCompareOp depthCompareOp);
    void SetZWrite(bool depthWriteEnable);
    void SetBlend(bool blendEnable, VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA, VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
    void SetBlendOp(VkBlendOp colorBlendOp);
    void SetColorMask(VkColorComponentFlags colorMask);
};