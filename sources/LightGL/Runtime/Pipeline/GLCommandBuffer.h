#pragma once
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

    GLCommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    GLCommandBuffer(const GLCommandBuffer&) = delete;
    ~GLCommandBuffer();

    void BeginRecording(VkCommandBufferUsageFlags flags = 0);
    void EndRecording() const;

    void CopyBuffer(const GLBuffer& source, const GLBuffer& buffer) const;
    void CopyBufferToImage(const GLBuffer& source, const GLImage& image) const;
    void BlitImage(VkImage source, VkRect2D sourceRect, VkImage destination, VkRect2D destinationRect) const;
    /**
     * 通过传输的方式清除图片值
     */
    void ClearColorImage(const VkImage& image, VkImageLayout imageLayout, float color[4]) const;
    /**
     * 通过传输的方式清除图片值
     */
    void ClearColorImage(const GLImage& glImage, float color[4]) const;
    /**
     * 通过传输的方式清除图片值
     */
    void ClearDepthStencilImage(const GLImage& glImage, float depth, uint32_t stencil) const;
    void TransitionImageLayout(
        const VkImage& image, VkImageLayout oldLayout, VkImageLayout newLayout,
        VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask,
        VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage) const;

    void BeginRenderPass(const GLRenderPass& glRenderPass, const GLFramebuffer& glFramebuffer) const;
    void EndRenderPass() const;

    /**
     * @brief 开始一段动态渲染
     *
     * 动态渲染是不需要创建 @c GLRenderPass 的渲染方式，其不支持多Pass功能，但创建 @c GLPipeline 也不再需要@c GLRenderPass 。<br/>
     * 由于@c GLPipeline 保存了管线状态信息，因此每个着色器（封装后的）都会有单独一个管线实例。如果使用基于Pass的渲染方式，则还都必须额外创建一个 @c GLRenderPass 。
     * 相比之下采用动态渲染方案则简单的多。
     * 
     * @param renderArea 渲染的视口区域
     * @param clearColor 渲染前是否要清除颜色（该功能利用@c LOAD_OP 实现，比传输或光栅化阶段清除更高效）。
     * @param colorView 渲染后输出到的颜色缓冲区
     * @param depthStencilView 渲染后输出到的深度缓冲区
     * @param colorResolveView 使用多重采用时，重结算颜色缓冲区后输出的无多重采样颜色缓冲区
     */
    void BeginRendering(
        VkRect2D renderArea, bool clearColor,
        const GLImageView& colorView, const GLImageView* depthStencilView, const GLImageView* colorResolveView
    ) const;
    void EndRendering() const;

    void BindPipeline(const GLPipeline& glPipeline) const;
    void BindVertexBuffers(const GLBuffer& glBuffer) const;
    void BindIndexBuffer(const GLBuffer& glBuffer) const;
    void BindDescriptorSets(const GLPipelineLayout& glPipelineLayout, const GLDescriptorSet& glDescriptorSet) const;
    void PushDescriptorSet(const GLPipelineLayout& glPipelineLayout, const std::vector<VkWriteDescriptorSet>& writeDescriptorSets) const;
    void PushConstant(const GLPipelineLayout& glPipelineLayout, const VkPushConstantRange& pushConstantRange, void* data) const;
    void SetViewport(float x, float y, float width, float height) const;
    void SetScissor(VkOffset2D offset, VkExtent2D extent) const;
    void DrawIndexed(uint32_t indicesCount) const;
    /**
     * 对当前绑定的帧缓冲区内容进行清除
     */
    void ClearAttachments(VkRect2D rect, float color[4], float depth, uint32_t stencil) const;
    void ExecuteSubCommands(const GLCommandBuffer& subCommandBuffer) const;

    /**
     * @brief 将命令缓冲区中记录的命令异步呈送到图形管道执行。
     * @param waitStages 
     * @param waitSemaphores 
     * @param signalSemaphores
     * @note 由于是异步执行，需要调用@c WaitSubmissionFinish() 来等待完成。
     */
    void SubmitCommandsAsync(const std::vector<VkPipelineStageFlags>& waitStages, const std::vector<VkSemaphore>& waitSemaphores,
                             const std::vector<VkSemaphore>& signalSemaphores);
    /**
     * @brief 将命令缓冲区中记录的命令呈送到图形管道并等待执行完毕。
     * @param waitStages 
     * @param waitSemaphores 
     */
    void SubmitCommands(const std::vector<VkPipelineStageFlags>& waitStages = {}, const std::vector<VkSemaphore>& waitSemaphores = {});
    void WaitSubmissionFinish();

private:
    VkCommandBufferLevel level;
    VkFence submissionFence;
    bool isSubmitting;
};
