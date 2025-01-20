#include "GShader.h"
#include "Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"

using namespace Light;

GShader::GShader(const std::string& shaderFile, const GLStateLayout& glStateLayout, const GLMeshLayout& glMeshLayout)
    : GShader(
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
GShader::GShader(const std::vector<GLShader>& shaderLayout, const GLStateLayout& glStateLayout, const GLMeshLayout& glMeshLayout)
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
void GShader::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GShader* lastShader) const
{
    if (this != lastShader)
        glCommandBuffer.BindPipeline(*glPipeline);
}
