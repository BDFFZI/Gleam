#include "GSLayout.h"

#include "GleamGraphics/Runtime/Graphics.h"
#include "GleamImport/Runtime/ShaderImporter.h"

namespace Gleam
{
    GSCodeLayout::GSCodeLayout(const std::vector<GLShader>& shaders): GLShaderLayout(shaders)
    {
    }
    GSCodeLayout::GSCodeLayout(const std::string& shaderFile): GLShaderLayout(std::vector{
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
    })
    {
    }
    
    GSAssetLayout::GSAssetLayout(std::vector<GLDescriptorBinding> descriptorBindings, std::vector<VkPushConstantRange> pushConstantRanges)
        : glDescriptorSetLayout(descriptorBindings, VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR),
          glPipelineLayout(glDescriptorSetLayout, pushConstantRanges),
          descriptorBindings(std::move(descriptorBindings)),
          pushConstantRanges(std::move(pushConstantRanges))
    {
    }
    const GLDescriptorSetLayout& GSAssetLayout::GetGlDescriptorSetLayout() const
    {
        return glDescriptorSetLayout;
    }
    const GLPipelineLayout& GSAssetLayout::GetGLPipelineLayout() const
    {
        return glPipelineLayout;
    }
    const std::vector<GLDescriptorBinding>& GSAssetLayout::GetDescriptorBindings() const
    {
        return descriptorBindings;
    }
    const std::vector<VkPushConstantRange>& GSAssetLayout::GetPushConstantRanges() const
    {
        return pushConstantRanges;
    }
    
    GSInoutLayout::GSInoutLayout(GLMeshLayout meshLayout, const VkFormat colorFormat, const VkFormat depthStencilFormat)
        : meshLayout(std::move(meshLayout)), colorFormat(colorFormat), depthStencilFormat(depthStencilFormat)
    {
    }
}