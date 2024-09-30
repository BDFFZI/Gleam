#pragma once
#include "CommandBuffer.h"
#include "LightGL/Runtime/GL.h"
#include "LightGL/Runtime/Pipeline/GLSwapChain.h"
#include "LightUtility/Runtime/ObjectPool.h"

namespace LightRuntime
{
    class Graphics
    {
    public:
        static void Initialize(GLFWwindow* window);
        static void UnInitialize();

        static const std::unique_ptr<GLSwapChain>& GetGLSwapChain();
        static float2 GetGLSwapChainExtent();
        static VkFormat GetPresentColorFormat();
        static VkFormat GetPresentDepthStencilFormat();
        static VkSampleCountFlagBits GetPresentSampleCount();
        static const std::unique_ptr<GLImageView>& GetPresentColorImageView();
        static const std::unique_ptr<GLImageView>& GetPresentDepthStencilImageView();
        static const std::unique_ptr<GLImageView>& GetPresentColorResolveImageView();

        static CommandBuffer* GetCommandBuffer(const std::string& name = "");
        static void ReleaseCommandBuffer(CommandBuffer* commandBuffer);

        static void PresentAsync(CommandBuffer& commandBuffer);
        /**
         * 等待上一次调用的PresentAsync完成
         */
        static void WaitPresent();
        static void WaitPresentAsync(const std::function<void()>& callback);
        /**
         * 等待到可再次执行PresentAsync
         */
        static void WaitPresentable();

    private:
        inline static std::unique_ptr<GLSwapChain> glSwapChain = {};
        inline static size_t glSwapChainBufferCount = {};
        inline static float2 glSwapChainExtent = {};

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
