#pragma once

#include <memory>

#include "../Pipeline/GLCommandBuffer.h"
#include "../Resource/GLImageView.h"

class GLSwapChain
{
public:
    static VkSurfaceCapabilitiesKHR QuerySurfaceCapabilitySupport();
    static std::vector<VkSurfaceFormatKHR> QuerySurfaceFormatSupport();
    static std::vector<VkPresentModeKHR> QueryPresentModeSupport();

    static VkSurfaceFormatKHR PickSwapSurfaceFormat(VkSurfaceFormatKHR desiredSurfaceFormat);
    static VkPresentModeKHR PickSwapPresentMode(VkPresentModeKHR desiredPresentMode);

    uint32_t minImageCount;
    uint32_t imageCount;
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> images = {};
    VkExtent2D imageExtent = {};
    VkFormat imageFormat = {};
    std::vector<std::unique_ptr<GLImageView>> imageViews = {};

    GLSwapChain(
        VkSurfaceFormatKHR surfaceFormat = PickSwapSurfaceFormat({VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}),
        VkPresentModeKHR presentMode = PickSwapPresentMode(VK_PRESENT_MODE_MAILBOX_KHR)
    );
    GLSwapChain(const GLSwapChain&) = delete;
    ~GLSwapChain();

    uint32_t GetCurrentBufferIndex() const { return currentBufferIndex; }
    uint32_t GetCurrentImageIndex() const { return currentImageIndex; }

    /**
     * 
     * @param outImageIndex 返回被用于呈现的图像视图，获取后用户需将呈现内容写入其中。
     * @param outBufferIndex 返回交换链内部当前要使用的缓冲区索引，用户在外部构建相同的缓冲区后（如命令缓冲区列表），可以此确定缓冲区可用性
     * @param outImageAvailableSemaphore 返回呈现图像可用时的GPU信号，写入内容时需先等待该信号激活
     * @param outPresentSemaphore 返回当内容已写入完毕，确实可以呈现时用户需要发出的GPU信号
     * @return 
     */
    bool SwitchImageAsync(uint32_t* outImageIndex, uint32_t* outBufferIndex, VkSemaphore* outImageAvailableSemaphore, VkSemaphore* outPresentSemaphore);
    void PresentImageAsync();

private:
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    uint32_t currentImageIndex = 0;
    uint32_t currentBufferIndex = 0;
};
