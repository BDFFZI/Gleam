#include "Shader.h"

#include "Mesh.h"

using namespace LightRuntime;

Shader::Shader(const std::vector<GLShader>& shaderLayout, const std::vector<GLDescriptorBinding>& descriptorSetLayout)
{
    glDescriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(descriptorSetLayout);
    glPipelineLayout = std::make_unique<GLPipelineLayout>(*glDescriptorSetLayout);
    glPipeline = std::make_unique<GLPipeline>(
        shaderLayout,
        Mesh::GetMeshLayout(),
        *this->glPipelineLayout);
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
