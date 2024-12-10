#pragma once
#include <functional>
#include "GLFramebuffer.h"
#include "GLPipeline.h"
#include "../Resource/GLBuffer.h"
#include  "../Resource/GLDescriptorSet.h"

/**
 * 带有KHR和EXT后缀的是扩展命令，目前不是vulkan的核心命令，需要一定前置要求来开启。
 * 其中KHR是官方扩展，支持性比较好；EXT是第三方提议的扩展，支持性较差。（来自AI的回答）
 */
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
    /**
     * 利用Barrier功能，转换图片的内存布局
     * @param image 
     * @param oldLayout 转换前的图片布局
     * @param newLayout 转换后的图片布局
     * @param srcAccessMask 表明必须在哪类操作完成后才可进行转换
     * @param dstAccessMask 表明哪类操作必须在转换完成后才可进行
     * @param srcStage 表明srcAccessMask所作用的着色器阶段
     * @param dstStage 表明dstAccessMask所作用的着色器阶段
     */
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
    void BindDescriptorSet(const GLPipelineLayout& glPipelineLayout, const GLDescriptorSet& glDescriptorSet) const;
    /**
     * @brief 不需要创建描述符集池和描述符集，而是利用命令缓冲区自身内存，动态绑定描述符
     * @param glPipelineLayout 
     * @param writeDescriptorSets
     * @note 需要启用 @c VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME 设备扩展以及相关功能特征
     */
    void PushDescriptorSetKHR(const GLPipelineLayout& glPipelineLayout, const std::vector<VkWriteDescriptorSet>& writeDescriptorSets) const;
    void PushConstant(const GLPipelineLayout& glPipelineLayout, const VkPushConstantRange& pushConstantRange, void* data) const;
    void SetViewport(float x, float y, float width, float height) const;
    void SetScissor(VkOffset2D offset, VkExtent2D extent) const;
    /**
     * @brief 动态设置多重采样次数
     * @param rasterizationSamples
     * @note 需要启用 @c VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME 设备扩展以及相关功能特征
     */
    void SetRasterizationSamplesEXT(VkSampleCountFlagBits rasterizationSamples) const;
    /**
    * @brief 动态设置顶点输入布局
    * @param vertexInput
    * @note 需要启用 @c VK_EXT_vertex_input_dynamic_state 设备扩展以及相关功能特征
    */
    void SetVertexInputEXT(const GLVertexInput& vertexInput) const;
    void SetInputAssembly(const GLInputAssembly& inputAssembly) const;
    void DrawIndexed(uint32_t indicesCount) const;
    /**
     * 对当前绑定的帧缓冲区内容进行清除
     */
    void ClearAttachments(VkRect2D rect, const std::optional<VkClearColorValue>& color, std::optional<VkClearDepthStencilValue> depthStencil) const;
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
