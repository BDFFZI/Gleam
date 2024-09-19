#include "GLCommandBuffer.h"

#include <array>
#include <stdexcept>

#include "../Foundation/GLFoundation.h"

void GLCommandBuffer::ExecuteSingleTimeCommands(const std::function<void(const GLCommandBuffer&)>& setCommands)
{
    GLCommandBuffer glCommandBuffer = {};
    glCommandBuffer.BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    setCommands(glCommandBuffer);
    glCommandBuffer.EndRecord();
    glCommandBuffer.ExecuteCommandBuffer();
}

GLCommandBuffer::GLCommandBuffer()
    : executing(false)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = GLFoundation::glCommandPool->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //直接用于提交到命令管道，而不是作为子命令由其他缓冲区调用
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(GLFoundation::glDevice->device, &allocInfo, &commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("创建命令缓冲区失败!");

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (vkCreateFence(GLFoundation::glDevice->device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
        throw std::runtime_error("创建信号围栏失败!");
}
GLCommandBuffer::~GLCommandBuffer()
{
    vkFreeCommandBuffers(GLFoundation::glDevice->device, GLFoundation::glCommandPool->commandPool, 1, &commandBuffer);
    vkDestroyFence(GLFoundation::glDevice->device, fence, nullptr);
}

void GLCommandBuffer::BeginRecord(const VkCommandBufferUsageFlags flags)
{
    //确保处于可用状态
    WaitExecutionFinish();
    //清除原内容
    vkResetCommandBuffer(commandBuffer, 0);
    //开始命令录制
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional
    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("开始命令录制失败!");
}
void GLCommandBuffer::EndRecord() const
{
    //真正提交命令，并检查命令是否错误
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("命令录制失败!");
}

void GLCommandBuffer::CopyBuffer(const GLBuffer& source, const GLBuffer& buffer) const
{
    VkBufferCopy copyRegion;
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = buffer.size;
    vkCmdCopyBuffer(commandBuffer, source.buffer, buffer.buffer, 1, &copyRegion);
}
void GLCommandBuffer::CopyBufferToImage(const GLBuffer& source, const GLImage& image) const
{
    VkBufferImageCopy region;
    //缓冲区信息
    {
        region.bufferOffset = 0;
        //像素布局，例如可以借此实现在行之间填充自定义数据，都为0表示像素为常规的紧密排序
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
    }
    //图片信息
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        image.width,
        image.height,
        1
    };
    //图片视图信息
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;


    vkCmdCopyBufferToImage(
        commandBuffer,
        source.buffer,
        image.image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );
}

void GLCommandBuffer::BeginRenderPass(const GLRenderPass& glRenderPass, const GLFramebuffer& glFramebuffer) const
{
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = glRenderPass.renderPass;
    renderPassInfo.framebuffer = glFramebuffer.framebuffer;
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = glFramebuffer.extent;
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = clearValues.size();
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
void GLCommandBuffer::EndRenderPass() const
{
    vkCmdEndRenderPass(commandBuffer);
}

void GLCommandBuffer::BindPipeline(const GLPipeline& glPipeline) const
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, glPipeline.pipeline);
}
void GLCommandBuffer::BindVertexBuffers(const GLBuffer& glBuffer) const
{
    const VkBuffer vertexBuffers[] = {glBuffer.buffer};
    constexpr VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}
void GLCommandBuffer::BindIndexBuffer(const GLBuffer& glBuffer) const
{
    vkCmdBindIndexBuffer(commandBuffer, glBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);
}
void GLCommandBuffer::BindDescriptorSets(const GLPipelineLayout& glPipelineLayout, const GLDescriptorSet& glDescriptorSet) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, glPipelineLayout.pipelineLayout,
                            0, 1, &glDescriptorSet.descriptorSet, 0, nullptr);
}
void GLCommandBuffer::SetViewportAndScissor(const VkExtent2D& extent) const
{
    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void GLCommandBuffer::Draw(const int indicesCount) const
{
    vkCmdDrawIndexed(commandBuffer, indicesCount, 1, 0, 0, 0);
}

void GLCommandBuffer::ExecuteCommandBufferAsync(const std::vector<VkPipelineStageFlags>& waitStages, const std::vector<VkSemaphore>& waitSemaphores,
                                                const std::vector<VkSemaphore>& signalSemaphores)
{
    //提交命令
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //在哪个阶段要进行等待，以及等待的信号量
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    //要提交的命令缓冲区
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    //完成后需要发出的信号量
    submitInfo.pSignalSemaphores = signalSemaphores.data();
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    if (vkQueueSubmit(GLFoundation::glDevice->graphicQueue, 1, &submitInfo, fence) != VK_SUCCESS)
        throw std::runtime_error("无法提交命令缓冲区!");

    executing = true;
}
void GLCommandBuffer::ExecuteCommandBuffer(const std::vector<VkPipelineStageFlags>& waitStages, const std::vector<VkSemaphore>& waitSemaphores)
{
    ExecuteCommandBufferAsync(waitStages, waitSemaphores, {});
    WaitExecutionFinish();
}
void GLCommandBuffer::WaitExecutionFinish()
{
    if (executing == false)
        return;

    vkWaitForFences(GLFoundation::glDevice->device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkResetFences(GLFoundation::glDevice->device, 1, &fence);
    executing = false;
}
