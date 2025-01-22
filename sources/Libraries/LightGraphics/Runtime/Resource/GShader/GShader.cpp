#include "GShader.h"

#include "LightGraphics/Runtime/Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"

using namespace Light;


GShader::GShader(const GSCodeLayout& codeLayout, const GSInoutLayout& inoutLayout, const GSStateLayout& stateLayout, const GSAssetLayout& assetLayout)
{
    glPipeline = std::make_unique<GLPipeline>(
        std::vector{inoutLayout.colorFormat}, inoutLayout.depthStencilFormat,
        codeLayout, inoutLayout.meshLayout, assetLayout.GetGLPipelineLayout(),
        stateLayout);
}
GShader::GShader(const GSCodeLayout& codeLayout, const GSStateLayout& stateLayout, const GSAssetLayout& assetLayout, const GSInoutLayout& inoutLayout)
{
    glPipeline = std::make_unique<GLPipeline>(
        std::vector{inoutLayout.colorFormat}, inoutLayout.depthStencilFormat,
        codeLayout, inoutLayout.meshLayout, assetLayout.GetGLPipelineLayout(),
        stateLayout);
}
void GShader::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GShader* lastShader) const
{
    if (this != lastShader)
        glCommandBuffer.BindPipeline(*glPipeline);
}