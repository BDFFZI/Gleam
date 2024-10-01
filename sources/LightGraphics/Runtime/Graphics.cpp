#include "Graphics.h"

#include "LightGL/Runtime/Pipeline/GLSwapChain.h"

using namespace LightRuntime;

void Graphics::Initialize(GLFWwindow* window)
{
    GL::Initialize(window);

    surfaceFormat = GLSwapChain::PickSwapSurfaceFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR});
    presentMode = GLSwapChain::PickSwapPresentMode(VK_PRESENT_MODE_MAILBOX_KHR);

    presentColorFormat = surfaceFormat.format;
    presentDepthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
    presentSampleCount = GL::glDevice->maxUsableSampleCount;

    CreateSwapChain();

    presentCommandBuffers.resize(glSwapChainBufferCount);
    for (size_t i = 0; i < glSwapChainBufferCount; ++i)
        presentCommandBuffers[i] = std::make_unique<GLCommandBuffer>();
}
void Graphics::UnInitialize()
{
    vkDeviceWaitIdle(GL::glDevice->device);
}

const std::unique_ptr<GLSwapChain>& Graphics::GetGLSwapChain()
{
    return glSwapChain;
}
float2 Graphics::GetGLSwapChainExtent()
{
    return glSwapChainExtent;
}
const std::unique_ptr<GLImageView>& Graphics::GetPresentColorImageView()
{
    return presentSampleCount == VK_SAMPLE_COUNT_1_BIT ? glSwapChain->GetCurrentImageView() : presentColorImageView;
}
const std::unique_ptr<GLImageView>& Graphics::GetPresentDepthStencilImageView()
{
    return presentDepthStencilImageView;
}
const std::unique_ptr<GLImageView>& Graphics::GetPresentColorResolveImageView()
{
    return presentSampleCount != VK_SAMPLE_COUNT_1_BIT ? glSwapChain->GetCurrentImageView() : presentColorImageView;
}
VkFormat Graphics::GetPresentColorFormat()
{
    return presentColorFormat;
}
VkFormat Graphics::GetPresentDepthStencilFormat()
{
    return presentDepthStencilFormat;
}
VkSampleCountFlagBits Graphics::GetPresentSampleCount()
{
    return presentSampleCount;
}

CommandBuffer& Graphics::GetCommandBuffer(const std::string& name)
{
    return commandBufferPool.Get();
}
void Graphics::ReleaseCommandBuffer(CommandBuffer& commandBuffer)
{
    commandBufferPool.Release(commandBuffer);
}

void Graphics::WaitPresentable()
{
    presentCommandBuffers[glSwapChain->GetCurrentBufferIndex()]->WaitExecutionFinish();
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

    //添加命令
    GLCommandBuffer& primaryCommandBuffer = *presentCommandBuffers[imageIndex];
    primaryCommandBuffer.BeginRecording();
    primaryCommandBuffer.ExecuteCommands(commandBuffer.GetGLCommandBuffer());
    primaryCommandBuffer.TransitionImageLayout(
        glSwapChain->images[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );
    primaryCommandBuffer.EndRecording();

    //提交命令
    primaryCommandBuffer.ExecuteCommandBufferAsync(
        {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, //在颜色输出阶段要进行等待
        {imageAvailable}, //等待到交换链的下一张图片可用时继续
        {renderFinishedSemaphores} //完成后发出渲染完成信号量
    );

    glSwapChain->PresentImageAsync();
}
void Graphics::WaitPresent()
{
    presentCommandBuffers[(glSwapChainBufferCount + glSwapChain->GetCurrentBufferIndex() - 1) % glSwapChainBufferCount]->WaitExecutionFinish();
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
    glSwapChainExtent = {
        static_cast<float>(glSwapChain->imageExtent.width),
        static_cast<float>(glSwapChain->imageExtent.height)
    };

    if (presentSampleCount != VK_SAMPLE_COUNT_1_BIT)
    {
        presentColorImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferColor(width, height, presentColorFormat, presentSampleCount));
        presentColorImageView = std::make_unique<GLImageView>(*presentColorImage, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    presentDepthStencilImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferDepth(width, height, presentDepthStencilFormat, presentSampleCount));
    presentDepthStencilImageView = std::make_unique<GLImageView>(*presentDepthStencilImage, VK_IMAGE_ASPECT_DEPTH_BIT);
}
