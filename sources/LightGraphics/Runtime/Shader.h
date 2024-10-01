#pragma once
#include <memory>

#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Pipeline/GLPipelineLayout.h"
#include "LightGL/Runtime/Pipeline/GLShader.h"
#include "LightGL/Runtime/Resource/GLDescriptorPool.h"

namespace LightRuntime
{
    class Shader
    {
    public:
        Shader(
            const std::vector<GLShader>& shaderLayout,
            const std::vector<GLDescriptorBinding>& descriptorSetLayout, const std::vector<VkPushConstantRange>& pushConstantRanges,
            const GLMeshLayout& meshLayout,
            VkFormat colorFormat, VkFormat depthStencilFormat,
            const MultisampleState& multisampleState);
        Shader(
            const std::vector<GLShader>& shaderLayout,
            const std::vector<GLDescriptorBinding>& descriptorSetLayout,
            const std::vector<VkPushConstantRange>& pushConstantRanges = {});
        Shader(const Shader&) = delete;

        const GLPipelineLayout& GetGLPipelineLayout() const;
        const GLPipeline& GetGLPipeline() const;

        const std::vector<GLDescriptorBinding>& GetDescriptorBinding() const;
        const std::vector<VkPushConstantRange>& GetPushConstantBinding() const;

    private:
        std::unique_ptr<GLDescriptorSetLayout> glDescriptorSetLayout;
        std::vector<VkPushConstantRange> pushConstantRanges;
        std::unique_ptr<GLPipelineLayout> glPipelineLayout;
        std::unique_ptr<GLPipeline> glPipeline;
    };
}
