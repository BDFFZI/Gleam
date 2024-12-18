#include "GLPipelineLayout.h"

#include <stdexcept>

#include "../GL.h"

GLPipelineLayout::GLPipelineLayout(const GLDescriptorSetLayout& glDescriptorSetLayout, const std::vector<VkPushConstantRange>& pushConstantRanges)
{
    uint32_t pushConstantSize = 0;
    for (const auto& pushConstantRange : pushConstantRanges)
        pushConstantSize += pushConstantRange.size;
    if (pushConstantSize > GL::glDevice->deviceProperties.limits.maxPushConstantsSize)
        throw std::runtime_error("推送常量总计大小超出显卡支持范围！");

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &glDescriptorSetLayout.descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();
    if (vkCreatePipelineLayout(GL::glDevice->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("创建管线布局失败！");
}
GLPipelineLayout::~GLPipelineLayout()
{
    vkDestroyPipelineLayout(GL::glDevice->device, pipelineLayout, nullptr);
}
