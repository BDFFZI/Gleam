#include "Shader.h"
#include "Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"

using namespace Light;

ShaderT::ShaderT(
    const std::vector<GLShader>& shaderLayout,
    const GLStateLayout& stateLayout, const GLMeshLayout& meshLayout,
    const VkFormat colorFormat, const VkFormat depthStencilFormat,
    const std::vector<GLDescriptorBinding>& descriptorBindings, VkDescriptorSetLayoutCreateFlags descriptorFlags,
    const std::vector<VkPushConstantRange>& pushConstantRanges
): pushConstantRanges(pushConstantRanges)
{
    glDescriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(descriptorBindings, descriptorFlags);
    glPipelineLayout = std::make_unique<GLPipelineLayout>(*glDescriptorSetLayout, pushConstantRanges);
    glPipeline = std::make_unique<GLPipeline>(
        std::vector{colorFormat}, depthStencilFormat,
        shaderLayout, meshLayout, *this->glPipelineLayout,
        stateLayout);
}

void ShaderT::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderBase* lastShader)
{
    glCommandBuffer.BindPipeline(*glPipeline);
}
