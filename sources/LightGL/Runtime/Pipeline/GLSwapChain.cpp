#include "GLSwapChain.h"

#include <algorithm>
#include <stdexcept>

#include "../GL.h"

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

VkSurfaceCapabilitiesKHR GLSwapChain::QuerySurfaceCapabilitySupport()
{
    const auto physicalDevice = GL::glDevice->physicalDevice;
    const auto surface = GL::glSurface->surface;

    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    return surfaceCapabilities;
}
std::vector<VkSurfaceFormatKHR> GLSwapChain::QuerySurfaceFormatSupport()
{
    const auto physicalDevice = GL::glDevice->physicalDevice;
    const auto surface = GL::glSurface->surface;

    std::vector<VkSurfaceFormatKHR> surfaceFormats = {};

    //设备对目标窗口支持的交换链纹理格式
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    surfaceFormats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());

    return surfaceFormats;
}
std::vector<VkPresentModeKHR> GLSwapChain::QueryPresentModeSupport()
{
    const auto physicalDevice = GL::glDevice->physicalDevice;
    const auto surface = GL::glSurface->surface;

    std::vector<VkPresentModeKHR> presentModes = {};

    //设备对目标窗口支持的交换链呈现模式
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

    return presentModes;
}

VkSurfaceFormatKHR GLSwapChain::PickSwapSurfaceFormat(const VkSurfaceFormatKHR desiredSurfaceFormat)
{
    const std::vector<VkSurfaceFormatKHR> surfaceFormats = QuerySurfaceFormatSupport();
    for (const auto& surfaceFormat : surfaceFormats)
    {
        if (surfaceFormat.format == desiredSurfaceFormat.format && surfaceFormat.colorSpace == desiredSurfaceFormat.colorSpace)
            return desiredSurfaceFormat;
    }

    return surfaceFormats[0];
}
VkPresentModeKHR GLSwapChain::PickSwapPresentMode(const VkPresentModeKHR desiredPresentMode)
{
    const std::vector<VkPresentModeKHR> presentModes = QueryPresentModeSupport();
    for (const auto& presentMode : presentModes)
    {
        if (presentMode == desiredPresentMode)
            return desiredPresentMode;
    }

    return presentModes[0];
}

GLSwapChain::GLSwapChain(const VkSurfaceFormatKHR surfaceFormat, const VkPresentModeKHR presentMode)
{
#ifdef _DEBUG
    VkSurfaceFormatKHR surfaceFormatAvailable = PickSwapSurfaceFormat(surfaceFormat);
    VkPresentModeKHR presentModeAvailable = PickSwapPresentMode(presentMode);
    if (surfaceFormatAvailable.format != surfaceFormat.format || surfaceFormatAvailable.colorSpace != surfaceFormat.colorSpace)
        throw std::runtime_error("不支持的交换链缓冲区格式！");
    if (presentModeAvailable != presentMode)
        throw std::runtime_error("不支持的交换链呈现模式！");
#endif

    const VkSurfaceCapabilitiesKHR surfaceCapabilities = QuerySurfaceCapabilitySupport();

    const VkExtent2D extent = ChooseSwapExtent(surfaceCapabilities, GL::glSurface->window);
    uint32_t minImageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount)
        minImageCount = surfaceCapabilities.maxImageCount;

    uint32_t queueFamilyIndices[] = {GL::glDevice->graphicQueueFamily, GL::glDevice->presentQueueFamily};

    VkSwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapChainCreateInfo.surface = GL::glSurface->surface;
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
    swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform; //不做图像变换
    swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; //不与其他窗口混合
    swapChainCreateInfo.clipped = VK_TRUE; //窗口被遮挡时允许裁剪像素
    swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkDevice vkDevice = GL::glDevice->device;

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
        if (vkCreateSemaphore(GL::glDevice->device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(GL::glDevice->device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS)
            throw std::runtime_error("创建信号量失败!");
    }
}
GLSwapChain::~GLSwapChain()
{
    for (size_t i = 0; i < images.size(); i++)
    {
        vkDestroySemaphore(GL::glDevice->device, imageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(GL::glDevice->device, renderFinishedSemaphores[i], nullptr);
    }
    imageViews.clear();
    images.clear();
    vkDestroySwapchainKHR(GL::glDevice->device, swapChain, nullptr);
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
        GL::glDevice->device, swapChain,
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
    vkQueuePresentKHR(GL::glDevice->presentQueue, &presentInfo);

    currentBufferIndex = (currentBufferIndex + 1) % images.size();
}
