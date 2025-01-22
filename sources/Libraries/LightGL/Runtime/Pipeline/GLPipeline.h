#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

#include "GLMeshLayout.h"
#include "GLRenderPass.h"
#include "GLShaderLayout.h"
#include "GLStateLayout.h"
#include "GLPipelineLayout.h"

class GLPipeline
{
public:
    VkPipeline pipeline;

    GLPipeline(
        const GLRenderPass& glRenderPass, int subpassIndex,
        const GLShaderLayout& glShaderLayout,
        const GLMeshLayout& glMeshLayout,
        const GLPipelineLayout& glPipelineLayout,
        const GLStateLayout& glStateLayout = {});
    /**
     * @brief 创建适用于动态渲染的管线
     * @param colorAttachments 
     * @param depthStencilAttachment 
     * @param glShaderLayout 
     * @param glMeshLayout 
     * @param glPipelineLayout 
     * @param glStateLayout 
     */
    GLPipeline(
        const std::vector<VkFormat>& colorAttachments,
        VkFormat depthStencilAttachment,
        const GLShaderLayout& glShaderLayout,
        const GLMeshLayout& glMeshLayout,
        const GLPipelineLayout& glPipelineLayout,
        const GLStateLayout& glStateLayout = {});
    GLPipeline(const GLPipeline&) = delete;
    ~GLPipeline();
};
