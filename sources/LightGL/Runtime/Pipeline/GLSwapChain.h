#pragma once

#include <memory>

#include "../Pipeline/GLCommandBuffer.h"
#include "../Resource/GLImageView.h"

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    std::vector<VkSurfaceFormatKHR> surfaceFormats = {};
    std::vector<VkPresentModeKHR> presentModes = {};
};

class GLSwapChain
{
public:
    VkSwapchainKHR swapChain = VK_NULL_HANDLE;
    VkFormat swapChainImageFormat = {};
    VkExtent2D swapChainImageExtent = {};
    std::vector<VkImage> swapChainImages = {};
    std::vector<std::unique_ptr<GLImageView>> swapChainImageViews = {};

    GLSwapChain();
    GLSwapChain(const GLSwapChain&) = delete;
    ~GLSwapChain();

    uint32_t GetCurrentBufferIndex() const;

    /**
     * 
     * @param outImageIndex 返回被用于呈现的图像视图，获取后用户需将呈现内容写入其中。
     * @param outBufferIndex
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
