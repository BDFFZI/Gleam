﻿#include "GLPipelineLayout.h"

#include <stdexcept>

#include "../Foundation/GLFoundation.h"

GLPipelineLayout::GLPipelineLayout(const GLDescriptorSetLayout& glDescriptorSetLayout)
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &glDescriptorSetLayout.descriptorSetLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // 不使用推送常量
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(GLFoundation::glDevice->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("创建管线布局失败！");
}
GLPipelineLayout::~GLPipelineLayout()
{
    vkDestroyPipelineLayout(GLFoundation::glDevice->device, pipelineLayout, nullptr);
}
