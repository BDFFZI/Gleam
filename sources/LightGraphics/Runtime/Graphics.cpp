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

    commandBuffers.resize(glSwapChainBufferCount);
    for (size_t i = 0; i < glSwapChainBufferCount; ++i)
        commandBuffers[i] = std::make_unique<CommandBuffer>();
}
void Graphics::UnInitialize()
{
    vkDeviceWaitIdle(GL::glDevice->device);
}

const std::unique_ptr<GLSwapChain>& Graphics::GetGLSwapChain()
{
    return glSwapChain;
}
const std::unique_ptr<GLImageView>& Graphics::GetPresentColorImageView()
{
    return presentSampleCount == VK_SAMPLE_COUNT_1_BIT ? glSwapChain->GetCurrentImageView() : colorImageView;
}
const std::unique_ptr<GLImageView>& Graphics::GetPresentDepthStencilImageView()
{
    return depthStencilImageView;
}
const std::unique_ptr<GLImageView>& Graphics::GetPresentColorResolveImageView()
{
    return presentSampleCount != VK_SAMPLE_COUNT_1_BIT ? glSwapChain->GetCurrentImageView() : colorImageView;
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


void Graphics::Present(const std::function<void(CommandBuffer& commandBuffer)>& addCommand)
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
    CommandBuffer& commandBuffer = *commandBuffers[imageIndex];
    commandBuffer.BeginRecording();
    addCommand(commandBuffer);
    commandBuffer.GetGLCommandBuffer().TransitionImageLayout(
        glSwapChain->images[imageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    );
    commandBuffer.EndRecording();

    //提交命令
    commandBuffer.GetGLCommandBuffer().ExecuteCommandBufferAsync(
        {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT}, //在颜色输出阶段要进行等待
        {imageAvailable}, //等待到交换链的下一张图片可用时继续
        {renderFinishedSemaphores} //完成后发出渲染完成信号量
    );

    glSwapChain->PresentImageAsync();
}
void Graphics::WaitPresent()
{
    commandBuffers[glSwapChain->GetCurrentBufferIndex()]->GetGLCommandBuffer().WaitExecutionFinish();
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
        colorImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferColor(width, height, presentColorFormat, presentSampleCount));
        colorImageView = std::make_unique<GLImageView>(*colorImage, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    depthStencilImage = std::unique_ptr<GLImage>(GLImage::CreateFrameBufferDepth(width, height, presentDepthStencilFormat, presentSampleCount));
    depthStencilImageView = std::make_unique<GLImageView>(*depthStencilImage, VK_IMAGE_ASPECT_DEPTH_BIT);
}
