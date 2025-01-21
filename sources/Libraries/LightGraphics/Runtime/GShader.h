#pragma once
#include <memory>

#include "GraphicsPreset.hpp"
#include "Mesh.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Pipeline/GLShader.h"

namespace Light
{

    class GShader
    {
    public:
        /**
         * @param shaderLayout 必须是符合GraphicsPreset要求的着色器
         * @param glStateLayout 
         * @param glMeshLayout 
         */
        GShader(const std::vector<GLShader>& shaderLayout,
                const GLStateLayout& glStateLayout = GraphicsPreset::DefaultStateLayout,
                const GLMeshLayout& glMeshLayout = GraphicsPreset::DefaultMeshLayout);
        /**
         * @brief 自动加载文件中名为@c VertexShader 和 @c FragmentShader 的顶点和片段着色器
         */
        GShader(const std::string& shaderFile,
                const GLStateLayout& glStateLayout = GraphicsPreset::DefaultStateLayout,
                const GLMeshLayout& glMeshLayout = GraphicsPreset::DefaultMeshLayout);
        GShader(const GShader&) = delete;

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GShader* lastShader) const;

    private:
        const std::vector<GLDescriptorBinding>* glDescriptorBindings;
        const std::vector<VkPushConstantRange>* glPushConstantRanges;
        const GLPipelineLayout* glPipelineLayout;
        const GLPipeline* glPipeline;
        std::unique_ptr<GLPipeline> pipeline;
    };
}