#include "Graphics.h"

#include "LightGL/Runtime/Pipeline/GLSwapChain.h"

using namespace Light;

void Graphics::InitializeGLDemand(std::vector<const char*>& extensions)
{
    extensions.push_back(VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME); //为了支持推送描述符，简化描述符集创建
}
Graphics Graphics::Initialize(GL&)
{
    surfaceFormat = GLSwapChain::PickSwapSurfaceFormat({DefaultColorFormat, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
    if (surfaceFormat.format != DefaultColorFormat)
        throw std::runtime_error("不支持的颜色格式！");
    presentMode = GLSwapChain::PickSwapPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);
    presentSampleCount = DefaultGLStateLayout.multisample.rasterizationSamples;

    TryCreateSwapChain();

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

CommandBuffer& Graphics::ApplyCommandBuffer(const std::string& name)
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
GLCommandBuffer* Graphics::BeginPresent()
{
    //获取交换链下次呈现使用的相关信息
    if (glSwapChain->SwitchImageAsync(
        &currentImageIndex, &currentBufferIndex,
        &currentImageAvailable, &currentRenderFinishedSemaphores
    ) == false) //交换链过时，需重建
    {
        vkDeviceWaitIdle(GL::glDevice->device);
        glSwapChain.reset();
        TryCreateSwapChain();
        return nullptr;
    }

    //更新渲染目标信息(交换链中有多张颜色缓冲区，每次都会切换)
    if (presentSampleCount == VK_SAMPLE_COUNT_1_BIT)
    {
        presentRenderTexture.glColorImage = &glSwapChain->images[currentImageIndex];
        presentRenderTexture.glColorImageView = glSwapChain->imageViews[currentImageIndex].get();
    }
    else
    {
        presentRenderTexture.glColorResolveImage = &glSwapChain->images[currentImageIndex];
        presentRenderTexture.glColorResolveImageView = glSwapChain->imageViews[currentImageIndex].get();
    }

    //添加命令
    GLCommandBuffer& primaryCommandBuffer = *presentCommandBuffers[currentImageIndex];
    primaryCommandBuffer.BeginRecording();
    return &primaryCommandBuffer;
}
void Graphics::EndPresent(GLCommandBuffer& presentCommandBuffer)
{
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
bool Graphics::Present(CommandBuffer& commandBuffer)
{
    GLCommandBuffer* primaryCommandBuffer = BeginPresent();
    if (primaryCommandBuffer != nullptr)
    {
        primaryCommandBuffer->ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());
        EndPresent(*primaryCommandBuffer);
        return true;
    }
    return false;
}
void Graphics::WaitPresent()
{
    size_t bufferIndex = (glSwapChainBufferCount + glSwapChain->GetCurrentBufferIndex() - 1) % glSwapChainBufferCount;
    presentCommandBuffers[bufferIndex]->WaitSubmissionFinish();
}

bool Graphics::TryCreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(GL::glSurface->window, &width, &height);
    if (width == 0 || height == 0)
        return false;

    glSwapChain = std::make_unique<GLSwapChain>(surfaceFormat, presentMode);
    glSwapChainBufferCount = glSwapChain->images.size();

    if (presentSampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        presentColorImage = GLImage::CreateFrameBufferColor(width, height, DefaultColorFormat, presentSampleCount);
        presentColorImageView = std::make_unique<GLImageView>(*presentColorImage, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    presentDepthStencilImage = GLImage::CreateFrameBufferDepth(width, height, DefaultDepthStencilFormat, presentSampleCount);
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
        presentRenderTexture.glColorImageView = glSwapChain->imageViews[glSwapChain->GetCurrentImageIndex()].get();
        presentRenderTexture.glColorResolveImage = nullptr;
        presentRenderTexture.glColorResolveImageView = nullptr;
    }
    else
    {
        presentRenderTexture.glColorImage = &presentColorImage->image;
        presentRenderTexture.glColorImageView = presentColorImageView.get();
        presentRenderTexture.glColorResolveImage = &glSwapChain->images[glSwapChain->GetCurrentImageIndex()];
        presentRenderTexture.glColorResolveImageView = glSwapChain->imageViews[glSwapChain->GetCurrentImageIndex()].get();
    }

    return true;
}
