#include "Shader.h"

#include "Graphics.h"
#include "Mesh.h"

using namespace LightRuntime;

Shader::Shader(
    const std::vector<GLShader>& shaderLayout,
    const std::vector<GLDescriptorBinding>& descriptorSetLayout,
    const GLMeshLayout& meshLayout,
    const VkFormat colorFormat, const VkFormat depthStencilFormat
)
{
    glDescriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(descriptorSetLayout);
    glPipelineLayout = std::make_unique<GLPipelineLayout>(*glDescriptorSetLayout);
    glPipeline = std::make_unique<GLPipeline>(
        std::vector{colorFormat}, depthStencilFormat,
        shaderLayout,
        meshLayout,
        *this->glPipelineLayout);
}
Shader::Shader(const std::vector<GLShader>& shaderLayout, const std::vector<GLDescriptorBinding>& descriptorSetLayout)
    : Shader(shaderLayout, descriptorSetLayout, Mesh::GetMeshLayout(),
             Graphics::GetPresentColorFormat(), Graphics::GetPresentDepthStencilFormat())
{
}

const GLDescriptorSetLayout& Shader::GetGLDescriptorSetLayout() const
{
    return *glDescriptorSetLayout;
}
const GLPipelineLayout& Shader::GetGLPipelineLayout() const
{
    return *glPipelineLayout;
}
const GLPipeline& Shader::GetGLPipeline() const
{
    return *glPipeline;
}
