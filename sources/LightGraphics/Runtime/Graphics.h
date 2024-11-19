#pragma once
#include "CommandBuffer.h"
#include "LightGL/Runtime/GL.h"
#include "LightGL/Runtime/Pipeline/GLSwapChain.h"
#include "LightUtility/Runtime/ObjectPool.hpp"

namespace Light
{
    struct PresentRenderTexture : RenderTextureBase
    {
        uint32_t width, height;
        VkSampleCountFlagBits sampleCount;
        const VkImage* glColorImage;
        const VkImage* glDepthStencilImage;
        const VkImage* glColorResolveImage;
        const GLImageView* glColorImageView;
        const GLImageView* glDepthStencilImageView;
        const GLImageView* glColorResolveImageView;

        PresentRenderTexture() = default;

        uint32_t GetWidth() const override { return width; }
        uint32_t GetHeight() const override { return height; }
        VkSampleCountFlagBits GetSampleCount() const override { return sampleCount; }
        const VkImage& GetVKColorImage() const override { return *glColorImage; }
        const VkImage* GetVkDepthStencilImage() const override { return glDepthStencilImage; }
        const VkImage* GetVkColorResolveImage() const override { return glColorResolveImage; }
        const GLImageView& GetGLColorImageView() const override { return *glColorImageView; }
        const GLImageView* GetGLDepthStencilImageView() const override { return glDepthStencilImageView; }
        const GLImageView* GetGLColorResolveImageView() const override { return glColorResolveImageView; }
    };

    class Graphics
    {
    public:
        static void InitializeGLDemand(std::vector<const char*>& extensions);
        static Graphics Initialize(GL&);
        static void UnInitialize();

        static const std::unique_ptr<GLSwapChain>& GetGLSwapChain() { return glSwapChain; }
        static PresentRenderTexture& GetPresentRenderTexture() { return presentRenderTexture; }

        static CommandBuffer& ApplyCommandBuffer(const std::string& name = "");
        static void ReleaseCommandBuffer(CommandBuffer& commandBuffer);

        /**
         * @brief 等待到可再次执行 @c PresentAsync()
         * 
         * 用于渲染的命令缓冲区存在上限（等同于交换链中帧缓冲区数量），若以异步的方式进行渲染，就存在没有命令缓冲区可用的情况，所以需要等待。
         * 使用该函数可以阻塞线程直到下次用于实现呈现绘制的命令缓冲区闲置。
         *
         * @note 如果你并非异步渲染，可以忽略该函数。
         */
        static void WaitPresentable();
        /**
         * 开始一次呈现操作。
         * @return 本次呈现操作将使用的命令缓冲区，该缓冲区的录制启用和关闭由系统自动控制。
         * @note 必须在之后调用 @c EndPresent() 来完成呈现。
         */
        static GLCommandBuffer& BeginPresent();
        /**
         * 结束 @c BeginPresent 引发的呈现操作。
         * @param presentCommandBuffer
         * @note 该函数使用异步呈现，必须调用@c WaitPresent() 来确保呈现完成。
         */
        static void EndPresent(GLCommandBuffer& presentCommandBuffer);
        /**
         * @brief 异步绘制并呈现画面
         * 
         * 本质是对 @c BeginPresent() 和 @c EndPresent() 的快速调用。 
         * @param commandBuffer 
         */
        static void Present(CommandBuffer& commandBuffer);
        /**
         * @brief 所有呈现都是异步执行的，每次使用玩呈现命令后必须调用该函数以确保完成。
         * 
         * 更准确的说，是等待绘制用的命令缓冲区执行完毕，可以安全的释放绘制资源的时候，但此时真正的呈现命令可能还未开始。<br/>
         * 如果每帧结束时都调用了@c WaitPresent()，那将退化为同步渲染，因此不存在缓冲区冲突的问题，这样就不用在每帧开始前调用@c WaitPresentable()
         */
        static void WaitPresent();

    private:
        inline static size_t glSwapChainBufferCount = {};

        inline static VkSurfaceFormatKHR surfaceFormat = {};
        inline static VkPresentModeKHR presentMode = {};
        inline static VkSampleCountFlagBits presentSampleCount = {};

        inline static std::vector<std::unique_ptr<GLCommandBuffer>> presentCommandBuffers; //传输呈现命令用的主缓冲区
        inline static std::unique_ptr<ObjectPool<CommandBuffer>> paintCommandBufferPool = {}; //供用户记录绘制命令的缓冲区
        inline static std::unique_ptr<GLSwapChain> glSwapChain = {};
        inline static std::unique_ptr<GLImage> presentColorImage = {};
        inline static std::unique_ptr<GLImage> presentDepthStencilImage = {};
        inline static std::unique_ptr<GLImageView> presentColorImageView = {}; //颜色或颜色解析视图
        inline static std::unique_ptr<GLImageView> presentDepthStencilImageView = {};
        inline static PresentRenderTexture presentRenderTexture;

        inline static uint32_t currentImageIndex;
        inline static uint32_t currentBufferIndex;
        inline static VkSemaphore currentImageAvailable;
        inline static VkSemaphore currentRenderFinishedSemaphores;

        static void CreateSwapChain();

        Graphics() = default;
    };
}
