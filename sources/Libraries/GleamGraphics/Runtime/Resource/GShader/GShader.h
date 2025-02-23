#pragma once
#include <memory>

#include "GSLayout.h"
#include "GleamGL/Runtime/GLCommandBuffer.h"
#include "GleamGraphics/Runtime/Graphics.h"

namespace Gleam
{
    class GShader
    {
    public:
        GShader(
            const GSCodeLayout& codeLayout,
            const GSStateLayout& stateLayout = Graphics::GetGraphicsConfig().defaultGSStateLayout,
            const GSAssetLayout& assetLayout = Graphics::GetGraphicsConfig().defaultGSAssetLayout,
            const GSInoutLayout& inoutLayout = Graphics::GetGraphicsConfig().defaultGSInoutLayout);
        GShader(
            const GSCodeLayout& codeLayout,
            const GSInoutLayout& inoutLayout,
            const GSStateLayout& stateLayout = Graphics::GetGraphicsConfig().defaultGSStateLayout,
            const GSAssetLayout& assetLayout = Graphics::GetGraphicsConfig().defaultGSAssetLayout
        );
        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GShader* lastShader) const;

    private:
        std::unique_ptr<GLPipeline> glPipeline;
    };
}