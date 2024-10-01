#include "Shader.h"

#include "Graphics.h"
#include "Mesh.h"

using namespace LightRuntime;

Shader::Shader(
    const std::vector<GLShader>& shaderLayout,
    const std::vector<GLDescriptorBinding>& descriptorSetLayout, const std::vector<VkPushConstantRange>& pushConstantRanges,
    const GLMeshLayout& meshLayout,
    const VkFormat colorFormat, const VkFormat depthStencilFormat,
    const MultisampleState& multisampleState
): pushConstantRanges(pushConstantRanges)
{
    glDescriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(descriptorSetLayout);
    glPipelineLayout = std::make_unique<GLPipelineLayout>(*glDescriptorSetLayout, pushConstantRanges);
    glPipeline = std::make_unique<GLPipeline>(
        std::vector{colorFormat}, depthStencilFormat,
        shaderLayout, meshLayout, *this->glPipelineLayout,
        multisampleState);
}
Shader::Shader(
    const std::vector<GLShader>& shaderLayout,
    const std::vector<GLDescriptorBinding>& descriptorSetLayout,
    const std::vector<VkPushConstantRange>& pushConstantRanges)
    : Shader(shaderLayout, descriptorSetLayout, pushConstantRanges, Mesh::GetMeshLayout(),
             Graphics::GetPresentColorFormat(), Graphics::GetPresentDepthStencilFormat(),
             MultisampleState{Graphics::GetPresentSampleCount()}
    )
{
}

const GLPipelineLayout& Shader::GetGLPipelineLayout() const
{
    return *glPipelineLayout;
}
const GLPipeline& Shader::GetGLPipeline() const
{
    return *glPipeline;
}

const std::vector<GLDescriptorBinding>& Shader::GetDescriptorBinding() const
{
    return glDescriptorSetLayout->descriptorBindings;
}
const std::vector<VkPushConstantRange>& Shader::GetPushConstantBinding() const
{
    return pushConstantRanges;
}
