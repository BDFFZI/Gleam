#pragma once
#include <memory>

#include "RenderPass.h"
#include "LightGL/Runtime/Pipeline/GLMeshLayout.h"
#include "LightGL/Runtime/Pipeline/GLPipeline.h"
#include "LightGL/Runtime/Pipeline/GLPipelineLayout.h"
#include "LightGL/Runtime/Pipeline/GLShader.h"
#include "LightGL/Runtime/Resource/GLDescriptorPool.h"
#include "LightGL/Runtime/Resource/GLDescriptorSet.h"

struct Pass
{
    const std::vector<GLShader>* glShaderLayout;
    const GLMeshLayout* glMeshLayout;
    const GLPipelineLayout* glPipelineLayout;

    Pass(const std::vector<GLShader>& glShaderLayout, const GLMeshLayout& glMeshLayout, const GLPipelineLayout& glPipelineLayout)
        : glShaderLayout(&glShaderLayout), glMeshLayout(&glMeshLayout), glPipelineLayout(&glPipelineLayout)
    {
    }
};

class Shader
{
public:
    Shader(
        const RenderPass& renderPass,
        const GLMeshLayout& glMeshLayout,
        const std::vector<GLShader>& glShaderLayout,
        const std::vector<GLDescriptorBinding>& glPipelineLayout)
    {
        glDescriptorSetLayout = std::make_unique<GLDescriptorSetLayout>(glPipelineLayout);
        this->glPipelineLayout = std::make_unique<GLPipelineLayout>(*glDescriptorSetLayout);

        int passCount = renderPass.GetPassCount();
        glPipelines.resize(passCount);
        for (int i = 0; i < passCount; i++)
        {
            glPipelines[i] = std::make_unique<GLPipeline>(
                renderPass.GetGLRenderPass(), i,
                glShaderLayout,
                glMeshLayout,
                *this->glPipelineLayout);
        }

        glDescriptorPool = std::make_unique<GLDescriptorPool>(*glDescriptorSetLayout, 1);
        glDescriptorSet = std::make_unique<GLDescriptorSet>(*glDescriptorPool, *glDescriptorSetLayout);
    }
    Shader(const Shader&) = delete;

    const GLPipelineLayout& GetGLPipelineLayout() const
    {
        return *glPipelineLayout;
    }
    const GLPipeline& GetGLPipeline(const int index) const
    {
        return *glPipelines[index];
    }
    const GLDescriptorSet& GetGLDescriptorSet() const
    {
        return *glDescriptorSet;
    }

    std::unique_ptr<GLDescriptorSetLayout> glDescriptorSetLayout;
    std::unique_ptr<GLPipelineLayout> glPipelineLayout;
    std::vector<std::unique_ptr<GLPipeline>> glPipelines;
    std::unique_ptr<GLDescriptorPool> glDescriptorPool;
    std::unique_ptr<GLDescriptorSet> glDescriptorSet;
};
