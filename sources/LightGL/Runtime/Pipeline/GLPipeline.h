#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GLMeshLayout.h"
#include "GLRenderPass.h"
#include "GLShader.h"
#include "GLPipelineLayout.h"

struct MultisampleState
{
    VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
};

class GLPipeline
{
public:
    VkPipeline pipeline;

    GLPipeline(
        const std::vector<GLShader>& glShaderLayout,
        const GLMeshLayout& glMeshLayout,
        const GLPipelineLayout& glPipelineLayout,
        const GLRenderPass* glRenderPass = nullptr, int subpassIndex = 0,
        MultisampleState multisampleState = {});
    GLPipeline(const GLPipeline&) = delete;
    ~GLPipeline();
};
