#include "GLPipelineLayout.h"

#include <stdexcept>

#include "../GL.h"

GLPipelineLayout::GLPipelineLayout(const GLDescriptorSetLayout& glDescriptorSetLayout, const std::vector<VkPushConstantRange>& pushConstantRanges)
{
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
