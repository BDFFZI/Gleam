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

struct RasterizationState
{
    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
    VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE;
};

struct StateLayout
{
    MultisampleState multisample;
    RasterizationState rasterization;
};

class GLPipeline
{
public:
    VkPipeline pipeline;

    GLPipeline(
        const GLRenderPass& glRenderPass, int subpassIndex,
        const std::vector<GLShader>& glShaderLayout,
        const GLMeshLayout& glMeshLayout,
        const GLPipelineLayout& glPipelineLayout,
        const StateLayout& stateLayout = {});
    GLPipeline(
        const std::vector<VkFormat>& colorAttachments,
        VkFormat depthStencilAttachment,
        const std::vector<GLShader>& glShaderLayout,
        const GLMeshLayout& glMeshLayout,
        const GLPipelineLayout& glPipelineLayout,
        const StateLayout& stateLayout = {});
    GLPipeline(const GLPipeline&) = delete;
    ~GLPipeline();
};
