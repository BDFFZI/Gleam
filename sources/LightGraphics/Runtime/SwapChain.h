#pragma once
#include <memory>
#include <vulkan/vulkan_core.h>

#include "RenderTarget.h"
#include "LightGL/Runtime/Pipeline/GLSwapChain.h"

namespace Light
{
    class SwapChain
    {
    public:
        static RenderTargetAsset& GetPresentRenderTarget() { return presentRenderTarget; }

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
         * 该函数会使系统向交互链申请呈现目标，若交换链失效，还会尝试进行重建，若无法重建，也会提供下次呈现所需的命令缓冲区，只是渲染图片
         * (这也意味着，在调用该函数之前，交换链数据是过期的，不应被使用！当然，你可能会发现，即使违背，画面也似乎显示正常，
         * 那是因为你一直抓着呈现执行的间歇在向当前屏幕上正使用的图片写入，这可能导致画面撕裂，而且等真正的呈现完成后，画面将变成黑屏)
         *
         * @param outPresentCommandBuffer 本次呈现操作将使用的命令缓冲区，该缓冲区的录制启用和关闭由系统自动控制。
         * @return 本次呈现结果是否有效。无效时依然可以呈现，但其结果不会被用户看到，例如窗口被最小化。
         * @note 若开始呈现成功，则必须在之后调用 @c EndPresent() 来完成呈现。
         * @note 若有命令缓冲区依赖交换链资源，应确保在执行该函数之后再录制相关命令。
         */
        static bool BeginPresent(GLCommandBuffer** outPresentCommandBuffer);
        /**
         * 结束 @c BeginPresent 引发的呈现操作。
         * @param presentCommandBuffer
         * @note 该函数使用异步呈现，必须调用@c WaitPresent() 来确保呈现完成。
         */
        static void EndPresent(GLCommandBuffer& presentCommandBuffer);
        /**
         * @brief 等待上一次的呈现操作所使用的命令缓冲区执行完毕
         * 
         * 该函数将堵塞线程以等待命令缓冲区执行完成，执行后便可安全释放上次绘制所使用的各种资源。但这不等于呈现操作已完成，真正的呈现命令此时甚至可能还未开始。<br/>
         * 如果每帧结束时都调用了@c WaitPresent()，那将退化为同步渲染，因此不存在缓冲区冲突的问题，这样就不用在每帧开始前调用@c WaitPresentable()
         */
        static void WaitPresent();

    private:
        friend class Graphics;

        //交换链
        inline static VkSurfaceFormatKHR surfaceFormat = {};
        inline static VkPresentModeKHR presentMode = {};
        inline static std::unique_ptr<GLSwapChain> glSwapChain = {};
        //深度模板附件-支持深度模板测试
        inline static VkFormat presentDepthStencilFormat = {};
        inline static std::unique_ptr<GLImage> presentDepthStencilImage = {};
        inline static std::unique_ptr<GLImageView> presentDepthStencilImageView = {};
        //多重采样时的颜色附件-支持多重采样抗锯齿
        inline static VkSampleCountFlagBits presentSampleCount = {};
        inline static std::unique_ptr<GLImage> presentMultiColorImage = {};
        inline static std::unique_ptr<GLImageView> presentMultiColorImageView = {};
        //渲染目标信息
        inline static RenderTargetAsset presentRenderTarget = {};
        //呈现资源信息
        inline static std::vector<std::unique_ptr<GLCommandBuffer>> presentCommandBuffers;
        inline static uint32_t currentImageIndex; //当前呈现用的图片索引
        inline static uint32_t currentBufferIndex; //当前呈现用的缓冲区索引
        inline static VkSemaphore currentImageAvailable; //当前呈现用图片有效时系统发出的信号
        inline static VkSemaphore currentRenderFinishedSemaphores; //当前呈现用的图片可以呈现到屏幕时（渲染完毕）要发给系统的信号

        static bool TryCreateSwapChain();

        static void Initialize(
            VkFormat presentColorFormat,
            VkFormat presentDepthStencilFormat = VK_FORMAT_UNDEFINED,
            VkSampleCountFlagBits presentSampleCount = VK_SAMPLE_COUNT_1_BIT
        );
        static void UnInitialize();
    };
}
