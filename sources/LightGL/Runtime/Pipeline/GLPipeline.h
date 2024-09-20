#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GLMeshLayout.h"
#include "GLRenderPass.h"
#include "GLShader.h"
#include "GLPipelineLayout.h"


class GLPipeline
{
public:
    VkPipeline pipeline;

    GLPipeline(const GLRenderPass& glRenderPass, int subpassIndex,
               const std::vector<GLShader>& glShaderLayout, const GLMeshLayout& glMeshLayout, const GLPipelineLayout& glPipelineLayout,
               VkSampleCountFlagBits sampleCount);
    GLPipeline(const GLPipeline&) = delete;
    ~GLPipeline();
};
