#include "GLCommandBuffer.h"

#include <array>
#include <stdexcept>

#include "../GL.h"

void GLCommandBuffer::ExecuteSingleTimeCommands(const std::function<void(const GLCommandBuffer&)>& setCommands)
{
    GLCommandBuffer glCommandBuffer = {};
    glCommandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    setCommands(glCommandBuffer);
    glCommandBuffer.EndRecording();
    glCommandBuffer.SubmitCommands();
}

GLCommandBuffer::GLCommandBuffer(const VkCommandBufferLevel level)
    : level(level), isSubmitting(false)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = GL::glCommandPool->commandPool;
    allocInfo.level = level;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(GL::glDevice->device, &allocInfo, &commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("创建命令缓冲区失败!");

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (vkCreateFence(GL::glDevice->device, &fenceInfo, nullptr, &submissionFence) != VK_SUCCESS)
        throw std::runtime_error("创建信号围栏失败!");
}
GLCommandBuffer::~GLCommandBuffer()
{
    vkFreeCommandBuffers(GL::glDevice->device, GL::glCommandPool->commandPool, 1, &commandBuffer);
    vkDestroyFence(GL::glDevice->device, submissionFence, nullptr);
}

void GLCommandBuffer::BeginRecording(const VkCommandBufferUsageFlags flags)
{
    //确保处于可用状态
    WaitSubmissionFinish();
    //清除原内容
    vkResetCommandBuffer(commandBuffer, 0);
    //开始命令录制
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = flags;

    VkCommandBufferInheritanceInfo inheritanceInfo{};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    beginInfo.pInheritanceInfo = level == VK_COMMAND_BUFFER_LEVEL_SECONDARY ? &inheritanceInfo : nullptr;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("开始命令录制失败!");
}
void GLCommandBuffer::EndRecording() const
{
    //真正提交命令，并检查命令是否错误
    if (const auto result = vkEndCommandBuffer(commandBuffer); result != VK_SUCCESS)
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
void GLCommandBuffer::BlitImage(const VkImage source, const VkRect2D sourceRect, const VkImage destination, const VkRect2D destinationRect) const
{
    VkImageBlit blit{};
    blit.srcOffsets[0] = {sourceRect.offset.x, sourceRect.offset.y, 0};
    blit.srcOffsets[1] = {static_cast<int32_t>(sourceRect.extent.width), static_cast<int32_t>(sourceRect.extent.height), 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = 0;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {destinationRect.offset.x, destinationRect.offset.y, 0};
    blit.dstOffsets[1] = {static_cast<int32_t>(destinationRect.extent.width), static_cast<int32_t>(destinationRect.extent.height), 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = 0;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(commandBuffer,
                   source, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   destination, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1, &blit,
                   VK_FILTER_LINEAR);
}
void GLCommandBuffer::ClearColorImage(const VkImage& image, const VkImageLayout imageLayout, float color[4]) const
{
    VkClearColorValue colorValue;
    std::memcpy(colorValue.float32, color, sizeof(float) * 4);
    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    vkCmdClearColorImage(commandBuffer, image, imageLayout, &colorValue, 1, &subresourceRange);
}
void GLCommandBuffer::ClearColorImage(const GLImage& glImage, float color[4]) const
{
    ClearColorImage(glImage.image, glImage.layout, color);
}
void GLCommandBuffer::ClearDepthStencilImage(const GLImage& glImage, const float depth, const uint32_t stencil) const
{
    VkClearDepthStencilValue depthStencilValue;
    depthStencilValue.depth = depth;
    depthStencilValue.stencil = stencil;
    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.levelCount = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount = 1;

    vkCmdClearDepthStencilImage(commandBuffer, glImage.image, glImage.layout, &depthStencilValue, 1, &subresourceRange);
}
void GLCommandBuffer::TransitionImageLayout(
    const VkImage& image, const VkImageLayout oldLayout, const VkImageLayout newLayout,
    const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask,
    const VkPipelineStageFlags srcStage, const VkPipelineStageFlags dstStage) const
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //转换图形内存布局
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    //不使用队列所有权转移功能
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //受影响的图形
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; //视作颜色纹理
    barrier.subresourceRange.levelCount = 1; //转换mipmap的数量
    barrier.subresourceRange.baseMipLevel = 0; //转换mipmap的起点
    barrier.subresourceRange.layerCount = 1; //非数组
    barrier.subresourceRange.baseArrayLayer = 0;
    //屏障执行前后需要等待的操作类型
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;

    vkCmdPipelineBarrier(
        commandBuffer,
        srcStage, //屏障要在哪个阶段后开始
        dstStage, //屏障要在哪个阶段前完成
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
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
void GLCommandBuffer::BeginRendering(
    const VkRect2D renderArea, const bool clearColor,
    const GLImageView& colorView, const GLImageView* depthStencilView, const GLImageView* colorResolveView) const
{
    VkRenderingAttachmentInfo colorAttachmentInfo{};
    colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    colorAttachmentInfo.imageView = colorView.imageView;
    colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    colorAttachmentInfo.loadOp = clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentInfo.clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    if (colorResolveView != nullptr)
    {
        colorAttachmentInfo.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        colorAttachmentInfo.resolveImageView = colorResolveView->imageView;
        colorAttachmentInfo.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkRenderingAttachmentInfo depthStencilAttachmentInfo{};
    depthStencilAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depthStencilAttachmentInfo.imageView = depthStencilView->imageView;
    depthStencilAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthStencilAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthStencilAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthStencilAttachmentInfo.clearValue.depthStencil = {1.0f, 0};

    VkRenderingInfo renderingInfo{};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    renderingInfo.flags = 0;
    renderingInfo.renderArea = renderArea;
    renderingInfo.layerCount = 1;
    renderingInfo.viewMask = 0;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = &colorAttachmentInfo;
    if (depthStencilView != nullptr)
    {
        renderingInfo.pDepthAttachment = &depthStencilAttachmentInfo;
        renderingInfo.pStencilAttachment = &depthStencilAttachmentInfo;
    }


    vkCmdBeginRendering(commandBuffer, &renderingInfo);
}
void GLCommandBuffer::EndRendering() const
{
    vkCmdEndRendering(commandBuffer);
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
void GLCommandBuffer::BindDescriptorSet(const GLPipelineLayout& glPipelineLayout, const GLDescriptorSet& glDescriptorSet) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, glPipelineLayout.pipelineLayout,
                            0, 1, &glDescriptorSet.descriptorSet, 0, nullptr);
}
void GLCommandBuffer::PushDescriptorSetKHR(const GLPipelineLayout& glPipelineLayout, const std::vector<VkWriteDescriptorSet>& writeDescriptorSets) const
{
    PFN_vkVoidFunction functionAddr = vkGetDeviceProcAddr(GL::glDevice->device, "vkCmdPushDescriptorSetKHR");
    PFN_vkCmdPushDescriptorSetKHR pushDescriptorSetKhr = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(functionAddr); // NOLINT(clang-diagnostic-cast-function-type-strict)

    pushDescriptorSetKhr(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        glPipelineLayout.pipelineLayout,
        0, //述符集布局中的第几组描述符集
        static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data());
}
void GLCommandBuffer::PushConstant(const GLPipelineLayout& glPipelineLayout, const VkPushConstantRange& pushConstantRange, void* data) const
{
    vkCmdPushConstants(
        commandBuffer, glPipelineLayout.pipelineLayout,
        pushConstantRange.stageFlags,
        pushConstantRange.offset,
        pushConstantRange.size,
        data);
}
void GLCommandBuffer::SetViewport(const float x, const float y, const float width, const float height) const
{
    VkViewport viewport;
    viewport.x = x;
    viewport.y = y;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}
void GLCommandBuffer::SetScissor(const VkOffset2D offset, const VkExtent2D extent) const
{
    VkRect2D scissor;
    scissor.offset = offset;
    scissor.extent = extent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void GLCommandBuffer::SetRasterizationSamplesEXT(const VkSampleCountFlagBits rasterizationSamples) const
{
    PFN_vkVoidFunction functionAddr = vkGetDeviceProcAddr(GL::glDevice->device, "vkCmdSetRasterizationSamplesEXT");
    PFN_vkCmdSetRasterizationSamplesEXT vkCmdSetRasterizationSamplesEXT = reinterpret_cast<PFN_vkCmdSetRasterizationSamplesEXT>(functionAddr); // NOLINT(clang-diagnostic-cast-function-type-strict)

    vkCmdSetRasterizationSamplesEXT(commandBuffer, rasterizationSamples);
}
void GLCommandBuffer::SetVertexInputEXT(const GLVertexInput& vertexInput) const
{
    PFN_vkVoidFunction functionAddr = vkGetDeviceProcAddr(GL::glDevice->device, "vkCmdSetVertexInputEXT");
    PFN_vkCmdSetVertexInputEXT vkCmdSetVertexInputEXT = reinterpret_cast<PFN_vkCmdSetVertexInputEXT>(functionAddr); // NOLINT(clang-diagnostic-cast-function-type-strict)

    vkCmdSetVertexInputEXT(
        commandBuffer,
        static_cast<uint32_t>(vertexInput.bindingDescription2EXTs.size()),
        vertexInput.bindingDescription2EXTs.data(),
        static_cast<uint32_t>(vertexInput.attributeDescription2EXTs.size()),
        vertexInput.attributeDescription2EXTs.data()
    );
}
void GLCommandBuffer::SetInputAssembly(const GLInputAssembly& inputAssembly) const
{
    vkCmdSetPrimitiveTopology(commandBuffer, inputAssembly.inputAssemblyState.topology);
    vkCmdSetPrimitiveRestartEnable(commandBuffer, inputAssembly.inputAssemblyState.primitiveRestartEnable);
}

void GLCommandBuffer::DrawIndexed(const uint32_t indicesCount) const
{
    vkCmdDrawIndexed(commandBuffer, indicesCount, 1, 0, 0, 0);
}
void GLCommandBuffer::ClearAttachments(const VkRect2D rect, const std::optional<VkClearColorValue>& color, const std::optional<VkClearDepthStencilValue> depthStencil) const
{
    VkClearAttachment clearAttachments[2] = {
        {VK_IMAGE_ASPECT_COLOR_BIT, {}, {}},
        {VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, {}, {}}
    };
    if (color.has_value())
        clearAttachments[0].clearValue.color = color.value();
    if (depthStencil.has_value())
        clearAttachments[1].clearValue.depthStencil = depthStencil.value();

    VkClearRect clearRect[2] = {
        {rect, 0, 1},
        {rect, 0, 1}
    };

    int length = depthStencil.has_value() && color.has_value() ? 2 : 1;
    int offset = color.has_value() ? 0 : 1;
    vkCmdClearAttachments(commandBuffer, length, clearAttachments + offset, length, clearRect + offset);
}
void GLCommandBuffer::ExecuteSubCommands(const GLCommandBuffer& subCommandBuffer) const
{
    vkCmdExecuteCommands(commandBuffer, 1, &subCommandBuffer.commandBuffer);
}

void GLCommandBuffer::SubmitCommandsAsync(const std::vector<VkPipelineStageFlags>& waitStages, const std::vector<VkSemaphore>& waitSemaphores,
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
    if (const auto result = vkQueueSubmit(GL::glDevice->graphicQueue, 1, &submitInfo, submissionFence); result != VK_SUCCESS)
        throw std::runtime_error("无法提交命令缓冲区!");

    isSubmitting = true;
}
void GLCommandBuffer::SubmitCommands(const std::vector<VkPipelineStageFlags>& waitStages, const std::vector<VkSemaphore>& waitSemaphores)
{
    SubmitCommandsAsync(waitStages, waitSemaphores, {});
    WaitSubmissionFinish();
}
void GLCommandBuffer::WaitSubmissionFinish()
{
    if (isSubmitting == false)
        return;

    vkWaitForFences(GL::glDevice->device, 1, &submissionFence, VK_TRUE, UINT64_MAX);
    vkResetFences(GL::glDevice->device, 1, &submissionFence);
    isSubmitting = false;
}
