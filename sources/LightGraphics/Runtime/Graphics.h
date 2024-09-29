#pragma once
#include "CommandBuffer.h"
#include "LightGL/Runtime/GL.h"
#include "LightGL/Runtime/Pipeline/GLSwapChain.h"

namespace LightRuntime
{
    class Graphics
    {
    public:
        static void Initialize(GLFWwindow* window);
        static void UnInitialize();

        static const std::unique_ptr<GLSwapChain>& GetGLSwapChain();
        static const std::unique_ptr<GLImageView>& GetPresentColorImageView();
        static const std::unique_ptr<GLImageView>& GetPresentDepthStencilImageView();
        static const std::unique_ptr<GLImageView>& GetPresentColorResolveImageView();
        static VkFormat GetPresentColorFormat();
        static VkFormat GetPresentDepthStencilFormat();
        static VkSampleCountFlagBits GetPresentSampleCount();

        static void Present(const std::function<void(CommandBuffer& commandBuffer)>& addCommand);
        static void WaitPresent();

    private:
        inline static std::unique_ptr<GLSwapChain> glSwapChain = {};
        inline static size_t glSwapChainBufferCount = {};

        inline static VkSurfaceFormatKHR surfaceFormat = {};
        inline static VkPresentModeKHR presentMode = {};
        inline static VkFormat presentColorFormat = {};
        inline static VkFormat presentDepthStencilFormat = {};
        inline static VkSampleCountFlagBits presentSampleCount = {};
        
        inline static std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;
        inline static std::unique_ptr<GLImage> colorImage = {};
        inline static std::unique_ptr<GLImage> depthStencilImage = {};
        inline static std::unique_ptr<GLImageView> colorImageView = {}; //颜色或颜色解析视图
        inline static std::unique_ptr<GLImageView> depthStencilImageView = {};

        static void CreateSwapChain();
    };
}
