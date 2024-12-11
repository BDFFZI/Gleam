#include "Shader.h"

#include "LightImport/Runtime/ShaderImporter.h"

namespace Light
{
    Shader::Shader(
        const std::string& shaderFile,
        const std::vector<GLDescriptorBinding>& descriptorSetLayout,
        const GLStateLayout& stateLayout,
        const VkFormat colorFormat,
        const VkFormat depthStencilFormat
    ): ShaderT(std::vector{
                   GLShader(
                       VK_SHADER_STAGE_VERTEX_BIT,
                       ShaderImporter::ImportHlslFromFile(
                           shaderFile, shaderc_vertex_shader, "VertexShader"
                       ), "VertexShader"
                   ),
                   GLShader(
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       ShaderImporter::ImportHlslFromFile(
                           shaderFile, shaderc_fragment_shader, "FragmentShader"
                       ), "FragmentShader")
               }, descriptorSetLayout, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR,
               std::vector<VkPushConstantRange>{{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstant)}},
               stateLayout, MeshLayout,
               colorFormat, depthStencilFormat)
    {
    }
}
