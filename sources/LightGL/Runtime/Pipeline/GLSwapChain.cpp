#include "GLSwapChain.h"

#include <algorithm>
#include <stdexcept>

#include "../Foundation/GLFoundation.h"

SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;

    //缓存设备对目标表面的支持功能
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.surfaceCapabilities);

    //缓存设备对目标窗口支持的交换链纹理格式
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    details.surfaceFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.surfaceFormats.data());

    //缓存设备对目标窗口支持的交换链呈现模式
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());

    return details;
}
VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
}

bool SwapChainSupportDetails::CheckSwapSurfaceFormat(const VkSurfaceFormatKHR surfaceFormat) const
{
    return std::ranges::any_of(surfaceFormats, [&](const VkSurfaceFormatKHR& availableFormat)
    {
        return availableFormat.format == surfaceFormat.format && availableFormat.colorSpace == surfaceFormat.colorSpace;
    });
}
bool SwapChainSupportDetails::CheckSwapPresentMode(const VkPresentModeKHR presentMode) const
{
    return std::ranges::any_of(presentModes, [&](const VkPresentModeKHR& availablePresentMode)
    {
        return availablePresentMode == presentMode;
    });
}

GLSwapChain::GLSwapChain(const VkSurfaceFormatKHR surfaceFormat, const VkPresentModeKHR presentMode)
{
    //获取显卡支持的最适合的交换链缓冲区格式、呈现模式、缓冲区大小、缓冲区数量。
    const SwapChainSupportDetails& swapChainSupport = QuerySwapChainSupport(GLFoundation::glDevice->physicalDevice, GLFoundation::glSurface->surface);

    if (swapChainSupport.CheckSwapSurfaceFormat(surfaceFormat) == false)
        throw std::runtime_error("不支持的交换链缓冲区格式！");
    if (swapChainSupport.CheckSwapPresentMode(presentMode) == false)
        throw std::runtime_error("不支持的交换链呈现模式！");

    const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.surfaceCapabilities, GLFoundation::glSurface->window);
    uint32_t minImageCount = swapChainSupport.surfaceCapabilities.minImageCount + 1;
    if (swapChainSupport.surfaceCapabilities.maxImageCount > 0 && minImageCount > swapChainSupport.surfaceCapabilities.maxImageCount)
        minImageCount = swapChainSupport.surfaceCapabilities.maxImageCount;

    uint32_t queueFamilyIndices[] = {GLFoundation::glDevice->graphicQueueFamily, GLFoundation::glDevice->presentQueueFamily};

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = GLFoundation::glSurface->surface;
    swapChainCreateInfo.minImageCount = minImageCount;
    swapChainCreateInfo.imageFormat = surfaceFormat.format;
    swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapChainCreateInfo.imageExtent = extent;
    swapChainCreateInfo.imageArrayLayers = 1; //每个图像的图层数量，非纹理数组始终为1
    swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT; //图像的目的，做颜色附件或接受其他图片的传输
    if (queueFamilyIndices[0] != queueFamilyIndices[1])
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //交换链中的图片支持多个队列族并发访问
        swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
        swapChainCreateInfo.queueFamilyIndexCount = 2;
    }
    else
    {
        swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //交换链缓冲区同时只能由一个队列族独占访问
        swapChainCreateInfo.pQueueFamilyIndices = nullptr;
        swapChainCreateInfo.queueFamilyIndexCount = 0;
    }

    swapChainCreateInfo.presentMode = presentMode;
    swapChainCreateInfo.preTransform = swapChainSupport.surfaceCapabilities.currentTransform; //不做图像变换
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //不与其他窗口混合
    swapChainCreateInfo.clipped = VK_TRUE; //窗口被遮挡时允许裁剪像素
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkDevice vkDevice = GLFoundation::glDevice->device;

    if (vkCreateSwapchainKHR(vkDevice, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS)
        throw std::runtime_error("创建交换链失败！");

    //提取出交换链中的图片
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(vkDevice, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(vkDevice, swapChain, &imageCount, images.data());

    //创建图片视图
    imageViews.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++)
        imageViews[i] = std::make_unique<GLImageView>(images[i], surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);

    imageFormat = surfaceFormat.format;
    imageExtent = extent;

    //创建信号量
    imageAvailableSemaphores.resize(imageCount);
    renderFinishedSemaphores.resize(imageCount);
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (size_t i = 0; i < imageCount; i++)
    {
        if (vkCreateSemaphore(GLFoundation::glDevice->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(GLFoundation::glDevice->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS)
            throw std::runtime_error("创建信号量失败!");
    }
}
GLSwapChain::~GLSwapChain()
{
    for (size_t i = 0; i < images.size(); i++)
    {
        vkDestroySemaphore(GLFoundation::glDevice->device, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(GLFoundation::glDevice->device, renderFinishedSemaphores[i], nullptr);
    }
    imageViews.clear();
    images.clear();
    vkDestroySwapchainKHR(GLFoundation::glDevice->device, swapChain, nullptr);
}

uint32_t GLSwapChain::GetCurrentBufferIndex() const
{
    return currentBufferIndex;
}
uint32_t GLSwapChain::GetCurrentImageIndex() const
{
    return currentImageIndex;
}
const std::unique_ptr<GLImageView>& GLSwapChain::GetCurrentImageView() const
{
    return imageViews[currentImageIndex];
}

bool GLSwapChain::SwitchImageAsync(uint32_t* outImageIndex, uint32_t* outBufferIndex,
                                   VkSemaphore* outImageAvailableSemaphore, VkSemaphore* outPresentSemaphore)
{
    VkSemaphore imageAvailableSemaphore = imageAvailableSemaphores[currentBufferIndex];
    VkSemaphore renderFinishedSemaphore = renderFinishedSemaphores[currentBufferIndex];

    //获取用于呈现的图片
    const VkResult result = vkAcquireNextImageKHR(
        GLFoundation::glDevice->device, swapChain,
        UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        //交换链已失效需重新生成
        return false;
    }
    if (result != VK_SUCCESS)
        throw std::runtime_error("无法获取下一个交换链呈现图片!");

    *outImageIndex = currentImageIndex;
    *outBufferIndex = currentBufferIndex;
    *outImageAvailableSemaphore = imageAvailableSemaphore;
    *outPresentSemaphore = renderFinishedSemaphore;

    return true;
}
void GLSwapChain::PresentImageAsync()
{
    //呈现画面
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    //等待到哪些信号量发出后执行
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentBufferIndex];
    presentInfo.waitSemaphoreCount = 1;
    //要呈现的交换链
    presentInfo.pSwapchains = &swapChain;
    presentInfo.swapchainCount = 1;
    //要呈现的图片索引
    presentInfo.pImageIndices = &currentImageIndex;
    presentInfo.pResults = nullptr; // Optional
    vkQueuePresentKHR(GLFoundation::glDevice->presentQueue, &presentInfo);

    currentBufferIndex = (currentBufferIndex + 1) % images.size();
}
