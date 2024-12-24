#include "Shader.h"
#include "Graphics.h"
#include "LightUtility/Runtime/Import/ShaderImporter.h"

using namespace Light;

Shader::Shader(const std::string& shaderFile, const GLStateLayout& glStateLayout, const GLMeshLayout& glMeshLayout)
    : Shader(
        std::vector{
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
        },
        glStateLayout, glMeshLayout
    )
{
}
Shader::Shader(const std::vector<GLShader>& shaderLayout, const GLStateLayout& glStateLayout, const GLMeshLayout& glMeshLayout)
{
    pipeline = std::make_unique<GLPipeline>(
        std::vector{GraphicsPreset::DefaultColorFormat}, GraphicsPreset::DefaultDepthStencilFormat,
        shaderLayout, glMeshLayout, *GraphicsPreset::DefaultGLPipelineLayout,
        glStateLayout);

    glDescriptorBindings = &GraphicsPreset::DefaultDescriptorBindings;
    glPushConstantRanges = &GraphicsPreset::DefaultPushConstantRanges;
    glPipelineLayout = GraphicsPreset::DefaultGLPipelineLayout.get();
    glPipeline = pipeline.get();
}
void Shader::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderAsset* lastShader)
{
    if (this != lastShader)
        ShaderAsset::BindToPipeline(glCommandBuffer, lastShader);
}
