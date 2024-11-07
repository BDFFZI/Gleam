#include "Graphics.h"

#include "LightGL/Runtime/Pipeline/GLSwapChain.h"

using namespace Light;

Graphics Graphics::Initialize(GL&)
{
    surfaceFormat = GLSwapChain::PickSwapSurfaceFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
    presentMode = GLSwapChain::PickSwapPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);

    presentColorFormat = surfaceFormat.format;
    presentDepthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    presentSampleCount = GL::glDevice->maxUsableSampleCount;

    CreateSwapChain();

    presentCommandBuffers.resize(glSwapChainBufferCount);
    for (size_t i = 0; i < glSwapChainBufferCount; ++i)
        presentCommandBuffers[i] = std::make_unique<GLCommandBuffer>();
    paintCommandBufferPool = std::make_unique<ObjectPool<CommandBuffer>>();

    return {};
}
void Graphics::UnInitialize()
{
    vkDeviceWaitIdle(GL::glDevice->device);
    paintCommandBufferPool.reset();
    presentCommandBuffers.clear();
    presentColorImageView.reset();
    presentDepthStencilImageView.reset();
    presentColorImage.reset();
    presentDepthStencilImage.reset();
    glSwapChain.reset();
}

CommandBuffer& Graphics::GetCommandBuffer(const std::string& name)
{
    return paintCommandBufferPool->Get();
}
void Graphics::ReleaseCommandBuffer(CommandBuffer& commandBuffer)
{
    paintCommandBufferPool->Release(commandBuffer);
}

void Graphics::WaitPresentable()
{
    presentCommandBuffers[glSwapChain->GetCurrentBufferIndex()]->WaitSubmissionFinish();
}
void Graphics::PresentAsync(CommandBuffer& commandBuffer)
{
    //获取交换链下次呈现使用的相关信息
    uint32_t imageIndex;
    uint32_t bufferIndex;
    VkSemaphore imageAvailable;
    VkSemaphore renderFinishedSemaphores;
    if (glSwapChain->SwitchImageAsync(
        &imageIndex, &bufferIndex,
        &imageAvailable, &renderFinishedSemaphores
    ) == false) //交换链过时，需重建
    {
        vkDeviceWaitIdle(GL::glDevice->device);
        glSwapChain.reset();
        CreateSwapChain();
        return;
    }

    //更新渲染目标信息(交换链中有多张颜色缓冲区，每次都会切换)
    if (presentSampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        presentRenderTexture.glColorImage = &glSwapChain->images[glSwapChain->GetCurrentImageIndex()];
        presentRenderTexture.glColorImageView = glSwapChain->GetCurrentImageView().get();
    }
    else
    {
        presentRenderTexture.glColorResolveImage = &glSwapChain->images[glSwapChain->GetCurrentImageIndex()];
        presentRenderTexture.glColorResolveImageView = glSwapChain->GetCurrentImageView().get();
    }

    //添加命令
    GLCommandBuffer& primaryCommandBuffer = *presentCommandBuffers[imageIndex];
    primaryCommandBuffer.BeginRecording();
    primaryCommandBuffer.ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());
    primaryCommandBuffer.TransitionImageLayout(
        glSwapChain->images[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );
    primaryCommandBuffer.EndRecording();

    //提交命令
    primaryCommandBuffer.SubmitCommandsAsync(
        {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, //在颜色输出阶段要进行等待
        {imageAvailable}, //等待到交换链的下一张图片可用时继续
        {renderFinishedSemaphores} //完成后发出渲染完成信号量
    );

    glSwapChain->PresentImageAsync();
}
void Graphics::WaitPresent()
{
    size_t bufferIndex = (glSwapChainBufferCount + glSwapChain->GetCurrentBufferIndex() - 1) % glSwapChainBufferCount;
    presentCommandBuffers[bufferIndex]->WaitSubmissionFinish();
}


void Graphics::CreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(GL::glSurface->window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(GL::glSurface->window, &width, &height);
        glfwWaitEvents();
    }

    glSwapChain = std::make_unique<GLSwapChain>(surfaceFormat, presentMode);
    glSwapChainBufferCount = glSwapChain->images.size();

    if (presentSampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        presentColorImage = GLImage::CreateFrameBufferColor(width, height, presentColorFormat, presentSampleCount);
        presentColorImageView = std::make_unique<GLImageView>(*presentColorImage, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    presentDepthStencilImage = GLImage::CreateFrameBufferDepth(width, height, presentDepthStencilFormat, presentSampleCount);
    presentDepthStencilImageView = std::make_unique<GLImageView>(*presentDepthStencilImage, VK_IMAGE_ASPECT_DEPTH_BIT);

    //重建PresentRenderTexture
    presentRenderTexture.width = glSwapChain->imageExtent.width;
    presentRenderTexture.height = glSwapChain->imageExtent.height;
    presentRenderTexture.sampleCount = presentSampleCount;
    presentRenderTexture.glDepthStencilImage = &presentDepthStencilImage->image;
    presentRenderTexture.glDepthStencilImageView = presentDepthStencilImageView.get();
    if (presentSampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        presentRenderTexture.glColorImage = &glSwapChain->images[glSwapChain->GetCurrentImageIndex()];
        presentRenderTexture.glColorImageView = glSwapChain->GetCurrentImageView().get();
        presentRenderTexture.glColorResolveImage = nullptr;
        presentRenderTexture.glColorResolveImageView = nullptr;
    }
    else
    {
        presentRenderTexture.glColorImage = &presentColorImage->image;
        presentRenderTexture.glColorImageView = presentColorImageView.get();
        presentRenderTexture.glColorResolveImage = &glSwapChain->images[glSwapChain->GetCurrentImageIndex()];
        presentRenderTexture.glColorResolveImageView = glSwapChain->GetCurrentImageView().get();
    }
}
