#pragma once
#include <memory>

#include "GShaderLayout.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Pipeline/GLShader.h"
#include "LightGraphics/Runtime/Graphics.h"

namespace Light
{
    class GShader
    {
    public:
        /**
         * @param shaderLayout
         * @param glShaders 必须是符合GraphicsPreset要求的着色器
         * @param graphicsPreset
         */
        GShader(const GShaderLayout& shaderLayout, const std::vector<GLShader>& glShaders, const GraphicsPreset& graphicsPreset = Graphics::GetDefaultGraphicsPreset());
        /**
         * @brief 自动加载文件中名为@c VertexShader 和 @c FragmentShader 的顶点和片段着色器
         */
        GShader(const GShaderLayout& shaderLayout, const std::string& shaderFile, const GraphicsPreset& graphicsPreset = Graphics::GetDefaultGraphicsPreset());
        GShader(const GShader&) = delete;

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GShader* lastShader) const;

    private:
        std::unique_ptr<GLPipeline> glPipeline;
    };
}