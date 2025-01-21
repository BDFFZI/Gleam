#include "GShader.h"
#include "LightImport/Runtime/ShaderImporter.h"

using namespace Light;

GShader::GShader(const GShaderLayout& shaderLayout, const std::vector<GLShader>& glShaders, const GraphicsPreset& graphicsPreset)
{
    glPipeline = std::make_unique<GLPipeline>(
        std::vector{graphicsPreset.colorFormat}, graphicsPreset.depthStencilFormat,
        glShaders, graphicsPreset.meshLayout, shaderLayout.GetGLPipelineLayout(),
        graphicsPreset.stateLayout);
}
GShader::GShader(const GShaderLayout& shaderLayout, const std::string& shaderFile, const GraphicsPreset& graphicsPreset)
    : GShader(
        shaderLayout,
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
        graphicsPreset
    )
{
}
void GShader::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GShader* lastShader) const
{
    if (this != lastShader)
        glCommandBuffer.BindPipeline(*glPipeline);
}