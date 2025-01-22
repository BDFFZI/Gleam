﻿#pragma once
#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Pipeline/GLPipelineLayout.h"
#include "LightGL/Runtime/Pipeline/GLShaderLayout.h"
#include "LightGL/Runtime/Pipeline/GLStateLayout.h"

namespace Light
{
    class GSCodeLayout : public GLShaderLayout
    {
    public:
        explicit GSCodeLayout(const std::vector<GLShader>& shaders);
        /**
         * @brief 自动加载文件中名为@c VertexShader 和 @c FragmentShader 的顶点和片段着色器
         */
        explicit GSCodeLayout(const std::string& shaderFile);
    };
    class GSAssetLayout
    {
    public:
        GSAssetLayout(
            std::vector<GLDescriptorBinding> descriptorBindings = {},
            std::vector<VkPushConstantRange> pushConstantRanges = {}
        );

        const GLDescriptorSetLayout& GetGlDescriptorSetLayout() const;
        const GLPipelineLayout& GetGLPipelineLayout() const;
        const std::vector<GLDescriptorBinding>& GetDescriptorBindings() const;
        const std::vector<VkPushConstantRange>& GetPushConstantRanges() const;

    private:
        GLDescriptorSetLayout glDescriptorSetLayout;
        GLPipelineLayout glPipelineLayout;
        std::vector<GLDescriptorBinding> descriptorBindings;
        std::vector<VkPushConstantRange> pushConstantRanges;
    };
    class GSInoutLayout
    {
    public:
        GLMeshLayout meshLayout;
        VkFormat colorFormat;
        VkFormat depthStencilFormat;

        GSInoutLayout(GLMeshLayout meshLayout, VkFormat colorFormat, VkFormat depthStencilFormat);
        explicit GSInoutLayout(const GLVertexInput& vertexInput, const GLInputAssembly& inputAssembly);
    };
    using GSStateLayout = GLStateLayout;
}