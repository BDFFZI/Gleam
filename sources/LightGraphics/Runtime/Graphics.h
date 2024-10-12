#pragma once
#include "CommandBuffer.h"
#include "LightGL/Runtime/GL.h"
#include "LightGL/Runtime/Pipeline/GLSwapChain.h"
#include "LightUtility/Runtime/ObjectPool.h"
#include "LightUtility/Runtime/ThreadPool.h"

namespace LightRuntime
{
    class Graphics
    {
    public:
        static void Initialize(GLFWwindow* window);
        static void UnInitialize();

        static const std::unique_ptr<GLSwapChain>& GetGLSwapChain();
        static Vector2 GetGLSwapChainExtent();
        static VkFormat GetPresentColorFormat();
        static VkFormat GetPresentDepthStencilFormat();
        static VkSampleCountFlagBits GetPresentSampleCount();
        static const std::unique_ptr<GLImageView>& GetPresentColorImageView();
        static const std::unique_ptr<GLImageView>& GetPresentDepthStencilImageView();
        static const std::unique_ptr<GLImageView>& GetPresentColorResolveImageView();

        static CommandBuffer& GetCommandBuffer(const std::string& name = "");
        static void ReleaseCommandBuffer(CommandBuffer& commandBuffer);

        /**
         * 等待到可再次执行PresentAsync()
         * 详细：等待到下次用于实现呈现绘制的命令缓冲区闲置。因为是异步渲染，所以命令缓冲区存在仍被其他帧占用的情况。
         */
        static void WaitPresentable();
        /**
         * 绘制并呈现画面
         * @param commandBuffer 
         */
        static void PresentAsync(CommandBuffer& commandBuffer);
        /**
         * 等待上一次调用的PresentAsync完成。
         * 详细：更准确的说是等待绘制用的命令缓冲区执行完毕，可以安全的释放绘制资源，但此时真正的呈现命令甚至还未开始。
         * 提示：如果每帧结束时都调用了WaitPresent()，那将退化为同步渲染，因此不存在缓冲区冲突的问题，那样就可不用每帧开始前调用WaitPresentable()
         */
        static void WaitPresent();

    private:
        inline static std::unique_ptr<GLSwapChain> glSwapChain = {};
        inline static size_t glSwapChainBufferCount = {};
        inline static Vector2 glSwapChainExtent = {};

        inline static VkSurfaceFormatKHR surfaceFormat = {};
        inline static VkPresentModeKHR presentMode = {};
        inline static VkFormat presentColorFormat = {};
        inline static VkFormat presentDepthStencilFormat = {};
        inline static VkSampleCountFlagBits presentSampleCount = {};

        inline static std::vector<std::unique_ptr<GLCommandBuffer>> presentCommandBuffers;
        inline static std::unique_ptr<GLImage> presentColorImage = {};
        inline static std::unique_ptr<GLImage> presentDepthStencilImage = {};
        inline static std::unique_ptr<GLImageView> presentColorImageView = {}; //颜色或颜色解析视图
        inline static std::unique_ptr<GLImageView> presentDepthStencilImageView = {};

        inline static ObjectPool<CommandBuffer> commandBufferPool = {};

        static void CreateSwapChain();
    };
}
