﻿#pragma once
#include <functional>
#include "GLFramebuffer.h"
#include "GLPipeline.h"
#include "../Resource/GLBuffer.h"
#include  "../Resource/GLDescriptorSet.h"

class GLCommandBuffer
{
public:
    static void ExecuteSingleTimeCommands(const std::function<void(const GLCommandBuffer&)>& setCommands);

    VkCommandBuffer commandBuffer;

    GLCommandBuffer();
    GLCommandBuffer(const GLCommandBuffer&) = delete;
    ~GLCommandBuffer();

    void BeginRecord(VkCommandBufferUsageFlags flags = 0);
    void EndRecord() const;

    void CopyBuffer(const GLBuffer& source, const GLBuffer& buffer) const;
    void CopyBufferToImage(const GLBuffer& source, const GLImage& image) const;

    void BeginRenderPass(const GLRenderPass& glRenderPass, const GLFramebuffer& glFramebuffer) const;
    void EndRenderPass() const;

    void BindPipeline(const GLPipeline& glPipeline) const;
    void BindVertexBuffers(const GLBuffer& glBuffer) const;
    void BindIndexBuffer(const GLBuffer& glBuffer) const;
    void BindDescriptorSets(const GLPipelineLayout& glPipelineLayout, const GLDescriptorSet& glDescriptorSet) const;
    void SetViewportAndScissor(const VkExtent2D& extent) const;
    void Draw(int indicesCount) const;

    void ExecuteCommandBufferAsync(const std::vector<VkPipelineStageFlags>& waitStages, const std::vector<VkSemaphore>& waitSemaphores,
                                   const std::vector<VkSemaphore>& signalSemaphores);
    void ExecuteCommandBuffer(const std::vector<VkPipelineStageFlags>& waitStages = {}, const std::vector<VkSemaphore>& waitSemaphores = {});
    void WaitExecutionFinish();

private:
    VkFence fence;
    bool executing;
};
