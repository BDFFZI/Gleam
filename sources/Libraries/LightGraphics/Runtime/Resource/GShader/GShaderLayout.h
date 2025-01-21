#pragma once
#include <memory>

#include "LightGL/Runtime/Pipeline/GLPipelineLayout.h"
#include "LightGL/Runtime/Resource/GLDescriptorSetLayout.h"

namespace Light
{
    class GShaderLayout
    {
    public:
        GShaderLayout(
            std::vector<GLDescriptorBinding> descriptorBindings = {},
            std::vector<VkPushConstantRange> pushConstantRanges = {}
        );

        const GLDescriptorSetLayout& GetGLDescriptorSetLayout() const
        {
            return glDescriptorSetLayout;
        }
        const GLPipelineLayout& GetGLPipelineLayout() const
        {
            return glPipelineLayout;
        }
        const std::vector<GLDescriptorBinding>& GetDescriptorBindings() const
        {
            return descriptorBindings;
        }
        const std::vector<VkPushConstantRange>& GetPushConstantRanges() const
        {
            return pushConstantRanges;
        }

    private:
        const GLDescriptorSetLayout glDescriptorSetLayout;
        const GLPipelineLayout glPipelineLayout;
        const std::vector<GLDescriptorBinding> descriptorBindings;
        const std::vector<VkPushConstantRange> pushConstantRanges;
    };
}