#include "SwapChain.h"

#include <stdexcept>

#include "LightGL/Runtime/GL.h"
#include "LightGL/Runtime/Pipeline/GLSwapChain.h"

namespace Light
{
    void SwapChain::WaitPresentable()
    {
        presentCommandBuffers[glSwapChain->GetCurrentBufferIndex()]->WaitSubmissionFinish();
    }
    bool SwapChain::BeginPresent(GLCommandBuffer** outPresentCommandBuffer)
    {
        bool canPresent = true;

        //获取交换链下次呈现使用的相关信息
        if (glSwapChain->SwitchImageAsync(
            &currentImageIndex, &currentBufferIndex,
            &currentImageAvailable, &currentRenderFinishedSemaphores
        ) == false) //交换链过时，需重建
        {
            if (TryCreateSwapChain())
                return BeginPresent(outPresentCommandBuffer);
            canPresent = false;
        }

        //更新渲染目标信息(交换链中有多张颜色缓冲区，每次都会切换)
        if (presentSampleCount == VK_SAMPLE_COUNT_1_BIT)
            presentRenderTarget.glColorImageView = glSwapChain->imageViews[currentImageIndex].get();
        else
            presentRenderTarget.glColorResolveImageView = glSwapChain->imageViews[currentImageIndex].get();

        //添加命令
        *outPresentCommandBuffer = presentCommandBuffers[currentBufferIndex].get();
        (*outPresentCommandBuffer)->BeginRecording();
        return canPresent;
    }
    void SwapChain::EndPresent(GLCommandBuffer& presentCommandBuffer)
    {
        //由于没有使用renderPass，因此布局变换得手动设置。每次渲染后图片布局默认为“未定义”，需在呈现前将布局调整为“呈现源”
        presentCommandBuffer.TransitionImageLayout(
            glSwapChain->images[currentImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        );
        presentCommandBuffer.EndRecording();

        //提交命令
        presentCommandBuffer.SubmitCommandsAsync(
            {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, //在颜色输出阶段要进行等待
            {currentImageAvailable}, //等待到交换链的下一张图片可用时继续
            {currentRenderFinishedSemaphores} //完成后发出渲染完成信号量
        );

        glSwapChain->PresentImageAsync();
    }
    void SwapChain::WaitPresent()
    {
        size_t bufferIndex = (glSwapChain->imageCount + glSwapChain->GetCurrentBufferIndex() - 1) % glSwapChain->imageCount;
        presentCommandBuffers[bufferIndex]->WaitSubmissionFinish();
    }

    bool SwapChain::TryCreateSwapChain()
    {
        //判断是否可以重建
        int width = 0, height = 0;
        glfwGetFramebufferSize(GL::glSurface->window, &width, &height);
        if (width == 0 || height == 0)
            return false;
        //释放旧交换链
        vkDeviceWaitIdle(GL::glDevice->device); //等待显卡空闲，从而确保旧资源不被占用
        glSwapChain.reset();
        //创建交换链
        glSwapChain = std::make_unique<GLSwapChain>(surfaceFormat, presentMode);
        //创建可选的深度模板附件
        if (presentDepthStencilFormat != VK_FORMAT_UNDEFINED)
        {
            presentDepthStencilImage = GLImage::CreateFrameBufferDepth(width, height, presentDepthStencilFormat, presentSampleCount);
            presentDepthStencilImageView = std::make_unique<GLImageView>(*presentDepthStencilImage, VK_IMAGE_ASPECT_DEPTH_BIT);
        }
        //创建可选的多重颜色附件
        if (presentSampleCount != VK_SAMPLE_COUNT_1_BIT)
        {
            presentMultiColorImage = GLImage::CreateFrameBufferColor(width, height, surfaceFormat.format, presentSampleCount);
            presentMultiColorImageView = std::make_unique<GLImageView>(*presentMultiColorImage, VK_IMAGE_ASPECT_COLOR_BIT);
        }
        //重建PresentRenderTexture信息
        presentRenderTarget.width = glSwapChain->imageExtent.width;
        presentRenderTarget.height = glSwapChain->imageExtent.height;
        presentRenderTarget.glDepthStencilImageView = presentDepthStencilImageView.get();
        presentRenderTarget.imageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        if (presentSampleCount == VK_SAMPLE_COUNT_1_BIT)
        {
            presentRenderTarget.glColorImageView = glSwapChain->imageViews[glSwapChain->GetCurrentImageIndex()].get();
            presentRenderTarget.glColorResolveImageView = nullptr;
        }
        else
        {
            presentRenderTarget.glColorImageView = presentMultiColorImageView.get();
            presentRenderTarget.glColorResolveImageView = glSwapChain->imageViews[glSwapChain->GetCurrentImageIndex()].get();
        }

        return true;
    }

    void SwapChain::Initialize(
        const VkFormat presentColorFormat, const VkFormat presentDepthStencilFormat, const VkSampleCountFlagBits presentSampleCount)
    {
        surfaceFormat = GLSwapChain::PickSwapSurfaceFormat({presentColorFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
        if (surfaceFormat.format != presentColorFormat)
            throw std::runtime_error("不支持的颜色格式！");

        presentMode = GLSwapChain::PickSwapPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);
        SwapChain::presentDepthStencilFormat = presentDepthStencilFormat;
        SwapChain::presentSampleCount = presentSampleCount;

        TryCreateSwapChain();

        presentCommandBuffers.resize(glSwapChain->imageCount);
        for (auto& element : presentCommandBuffers)
            element = std::make_unique<GLCommandBuffer>();
    }
    void SwapChain::UnInitialize()
    {
        glSwapChain.reset();
        presentDepthStencilImage.reset();
        presentDepthStencilImageView.reset();
        presentMultiColorImage.reset();
        presentMultiColorImageView.reset();
        presentCommandBuffers.clear();
    }
}
