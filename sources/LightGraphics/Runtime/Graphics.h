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
         * @brief 等待到下次执行呈现操作所用的资源已可用
         * 
         * 用于渲染的命令缓冲区存在上限（等同于交换链中帧缓冲区数量），若以异步的方式进行渲染，就存在没有命令缓冲区可用的情况，所以需要等待。
         * 使用该函数可以阻塞线程直到下次用于实现呈现绘制的命令缓冲区闲置。
         *
         * @note 和 @c WaitPresent 比较，两者相当于两种不同的等待策略，所以如果你选择同步渲染，则可忽略该函数。
         */
        static void WaitPresentable();
        /**
         * 向交换链申请开始一段呈现操作。
         *
         * 该函数会使系统向交互链申请呈现目标，若交换链失效，还会尝试进行重建。
         * @return 本次呈现操作将使用的命令缓冲区，该缓冲区的录制启用和关闭由系统自动控制，若此时无法进行呈现操作，则返回空指针。
         * @note 若开始呈现成功，则必须在之后调用 @c EndPresent() 来完成呈现。
         * @note 若有命令缓冲区依赖交换链资源，应确保在执行该函数之后再录制相关命令。
         */
        static GLCommandBuffer* BeginPresent();
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
        static bool Present(CommandBuffer& commandBuffer);
        /**
         * @brief 等待上一次的呈现操作所使用的命令缓冲区执行完毕
         * 
         * 该函数将堵塞线程以等待命令缓冲区执行完成，执行后便可安全释放上次绘制所使用的各种资源。但这不等于呈现操作已完成，真正的呈现命令此时甚至可能还未开始。<br/>
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

        static bool TryCreateSwapChain();

        Graphics() = default;
    };
}
