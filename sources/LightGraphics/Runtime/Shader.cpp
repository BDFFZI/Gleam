#include "Shader.h"
#include "Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"

using namespace Light;

std::unique_ptr<Shader> Shader::CreateFromFile(
    const std::string& shaderFile,
    const std::vector<GLDescriptorBinding>& descriptorSetLayout,
    const GLStateLayout& stateLayout, const GLMeshLayout& meshLayout)
{
    std::vector<std::byte> vertexShader = ShaderImporter::ImportHlslFromFile(shaderFile, shaderc_vertex_shader, "VertexShader");
    std::vector<std::byte> fragmentShader = ShaderImporter::ImportHlslFromFile(shaderFile, shaderc_fragment_shader, "FragmentShader");

    return std::make_unique<Shader>(
        std::vector{
            GLShader(VK_SHADER_STAGE_VERTEX_BIT, vertexShader, "VertexShader"),
            GLShader(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader, "FragmentShader"),
        },
        descriptorSetLayout, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
        std::vector<VkPushConstantRange>{{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(BuiltInPushConstant)}},
        stateLayout, meshLayout,
        BuiltInColorFormat, BuiltInDepthStencilFormat
    );
}
Shader::Shader(
    const std::vector<GLShader>& shaderLayout,
    const std::vector<GLDescriptorBinding>& descriptorBindings, VkDescriptorSetLayoutCreateFlags descriptorFlags,
    const std::vector<VkPushConstantRange>& pushConstantRanges,
    const GLStateLayout& stateLayout, const GLMeshLayout& meshLayout,
    const VkFormat colorFormat, const VkFormat depthStencilFormat
): pushConstantRanges(pushConstantRanges)
{
    glDescriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(descriptorBindings, descriptorFlags);
    glPipelineLayout = std::make_unique<GLPipelineLayout>(*glDescriptorSetLayout, pushConstantRanges);
    glPipeline = std::make_unique<GLPipeline>(
        std::vector{colorFormat}, depthStencilFormat,
        shaderLayout, meshLayout, *this->glPipelineLayout,
        stateLayout);
}

void Shader::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderBase* lastShader)
{
    glCommandBuffer.BindPipeline(*glPipeline);
}
