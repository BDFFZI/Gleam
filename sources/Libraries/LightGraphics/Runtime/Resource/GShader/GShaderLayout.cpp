#include "GShaderLayout.h"

namespace Light
{
    GShaderLayout::GShaderLayout(
        std::vector<GLDescriptorBinding> descriptorBindings,
        std::vector<VkPushConstantRange> pushConstantRanges):
        glDescriptorSetLayout(descriptorBindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR),
        glPipelineLayout(glDescriptorSetLayout, pushConstantRanges),
        descriptorBindings(std::move(descriptorBindings)),
        pushConstantRanges(std::move(pushConstantRanges))
    {
    }
}