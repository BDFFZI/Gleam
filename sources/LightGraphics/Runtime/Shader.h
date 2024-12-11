#pragma once
#include <memory>

#include "Mesh.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Pipeline/GLPipelineLayout.h"
#include "LightGL/Runtime/Pipeline/GLShader.h"
#include "LightGL/Runtime/Resource/GLDescriptorPool.h"

namespace Light
{
    class ShaderBase
    {
    public:
        virtual ~ShaderBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderBase* lastShader) = 0;
        virtual const GLPipelineLayout& GetGLPipelineLayout() const = 0;
        virtual const std::vector<GLDescriptorBinding>& GetDescriptorBinding() const = 0;
        virtual const std::vector<VkPushConstantRange>& GetPushConstantBinding() const = 0;
    };

    class ShaderT : public ShaderBase
    {
    public:
        ShaderT(
            const std::vector<GLShader>& shaderLayout,
            const GLStateLayout& stateLayout, const GLMeshLayout& meshLayout,
            VkFormat colorFormat, VkFormat depthStencilFormat,
            const std::vector<GLDescriptorBinding>& descriptorBindings, VkDescriptorSetLayoutCreateFlags descriptorFlags,
            const std::vector<VkPushConstantRange>& pushConstantRanges);
        ShaderT(const ShaderT&) = delete;

        const GLPipelineLayout& GetGLPipelineLayout() const override { return *glPipelineLayout; }
        const std::vector<GLDescriptorBinding>& GetDescriptorBinding() const override { return glDescriptorSetLayout->descriptorBindings; }
        const std::vector<VkPushConstantRange>& GetPushConstantBinding() const override { return pushConstantRanges; }

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderBase* lastShader) override;

    private:
        std::unique_ptr<GLDescriptorSetLayout> glDescriptorSetLayout;
        std::vector<VkPushConstantRange> pushConstantRanges;
        std::unique_ptr<GLPipelineLayout> glPipelineLayout;
        std::unique_ptr<GLPipeline> glPipeline;
    };
}
