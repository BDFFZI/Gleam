#pragma once
#include <memory>

#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Pipeline/GLPipelineLayout.h"
#include "LightGL/Runtime/Pipeline/GLShader.h"
#include "LightGL/Runtime/Resource/GLDescriptorPool.h"
#include "LightMath/Runtime/Matrix.hpp"

namespace Light
{
    struct PushConstant
    {
        alignas(16) float4x4 objectToWorld;
        alignas(16) float4x4 worldToView;
        alignas(16) float4x4 viewToClip;
    };

    class Shader
    {
    public:
        static std::unique_ptr<Shader> CreateFromFile(
            const std::string& shaderFile,
            const std::vector<GLDescriptorBinding>& descriptorSetLayout = {},
            const StateLayout& stateLayout = {}
        );

        Shader(
            const std::vector<GLShader>& shaderLayout,
            const std::vector<GLDescriptorBinding>& descriptorBindings, VkDescriptorSetLayoutCreateFlags descriptorFlags,
            const std::vector<VkPushConstantRange>& pushConstantRanges,
            const GLMeshLayout& meshLayout,
            VkFormat colorFormat, VkFormat depthStencilFormat,
            const StateLayout& stateLayout);
        Shader(
            const std::vector<GLShader>& shaderLayout,
            const std::vector<GLDescriptorBinding>& descriptorSetLayout = {},
            const StateLayout& stateLayout = {});
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
