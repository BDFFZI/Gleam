#pragma once
#include <memory>

#include "GraphicsPreset.h"
#include "Mesh.h"
#include "../GL/Pipeline/GLPipeline.h"
#include "../GL/Pipeline/GLShader.h"

namespace Light
{
    class Shader : public ShaderAsset
    {
    public:
        /**
         * @param shaderLayout 必须是符合GraphicsPreset要求的着色器
         * @param glStateLayout 
         * @param glMeshLayout 
         */
        Shader(const std::vector<GLShader>& shaderLayout,
               const GLStateLayout& glStateLayout = GraphicsPreset::DefaultStateLayout,
               const GLMeshLayout& glMeshLayout = GraphicsPreset::DefaultMeshLayout);
        /**
         * @brief 自动加载文件中名为@c VertexShader 和 @c FragmentShader 的顶点和片段着色器
         */
        Shader(const std::string& shaderFile,
               const GLStateLayout& glStateLayout = GraphicsPreset::DefaultStateLayout,
               const GLMeshLayout& glMeshLayout = GraphicsPreset::DefaultMeshLayout);
        Shader(const Shader&) = delete;
        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderAsset* lastShader) override;

    private:
        std::unique_ptr<GLPipeline> pipeline;
    };
}
