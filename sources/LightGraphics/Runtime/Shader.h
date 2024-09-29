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
            const std::vector<GLDescriptorBinding>& descriptorSetLayout,
            const GLMeshLayout& meshLayout,
            VkFormat colorFormat, VkFormat depthStencilFormat);
        Shader(
            const std::vector<GLShader>& shaderLayout,
            const std::vector<GLDescriptorBinding>& descriptorSetLayout);
        Shader(const Shader&) = delete;

        const GLDescriptorSetLayout& GetGLDescriptorSetLayout() const;
        const GLPipelineLayout& GetGLPipelineLayout() const;
        const GLPipeline& GetGLPipeline() const;

    private:
        std::unique_ptr<GLDescriptorSetLayout> glDescriptorSetLayout;
        std::unique_ptr<GLPipelineLayout> glPipelineLayout;
        std::unique_ptr<GLPipeline> glPipeline;
    };
}
