#include "GLDevice.h"

#include <optional>
#include <set>
#include <stdexcept>

bool CheckDeviceFeaturesSupport(const VkPhysicalDevice device, const VkPhysicalDeviceFeatures& necessaryFeatures)
{
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    auto* supported = reinterpret_cast<VkBool32*>(&supportedFeatures);
    auto* necessary = reinterpret_cast<const VkBool32*>(&necessaryFeatures);
    constexpr size_t count = sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32);
    for (size_t i = 0; i < count; i++)
    {
        if (necessary[i] == VK_TRUE && supported[i] == VK_FALSE)
            return false;
    }

    return true;
}
bool CheckDeviceExtensionSupport(const VkPhysicalDevice device, const std::vector<const char*>& necessaryExtensions)
{
    //获取显卡支持的功能扩展
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    //若所需扩展在支持列表中，则从需求中划去
    std::set<std::string> requiredExtensions(necessaryExtensions.begin(), necessaryExtensions.end());
    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    //所有需求都被划去，说明完全支持
    return requiredExtensions.empty();
}
std::vector<uint32_t> FindQueueFamilies(const VkPhysicalDevice device, const std::vector<VkQueueFlagBits>& queueFlags)
{
    //获取设备支持的队列族
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    //寻找满足要求的队列组
    std::vector<uint32_t> queueFamilyIndices(queueFlags.size());
    for (size_t flagIndex = 0; flagIndex < queueFlags.size(); ++flagIndex)
    {
        size_t familyIndex = 0;
        for (; familyIndex < queueFamilyCount; ++familyIndex)
        {
            if (queueFamilies[familyIndex].queueFlags & queueFlags[flagIndex])
            {
                queueFamilyIndices[flagIndex] = static_cast<uint32_t>(familyIndex);
                break;
            }
        }
        if (familyIndex == queueFamilyCount)
            return {}; //无法找到满足要求的队列族
    }

    return queueFamilyIndices;
}
std::optional<uint32_t> FindPresetQueueFamily(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        //选择支持呈现命令的队列族（因为是扩展功能，所以要通过扩展函数判断）
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport)
            return i;
    }

    return {};
}
VkSampleCountFlagBits GetMaxUsableSampleCount(const VkPhysicalDeviceProperties& physicalDeviceProperties)
{
    VkSampleCountFlags counts =
        physicalDeviceProperties.limits.framebufferColorSampleCounts &
        physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

GLDevice::GLDevice(const GLInstance& glInstance, const GLSurface& glSurface)
{
    //获取支持vulkan的所有物理显卡
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(glInstance.instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(glInstance.instance, &deviceCount, devices.data());

    //设备必须支持的功能
    std::vector queueFlags = {VK_QUEUE_GRAPHICS_BIT}; //需支持图形管道
    VkPhysicalDeviceFeatures necessaryFeatures = {};
    necessaryFeatures.samplerAnisotropy = VK_TRUE; //需支持各向异性
    necessaryFeatures.sampleRateShading = VK_TRUE; //需支持着色多重采样（改善来自纹理中的锯齿）
    //需支持的设备扩展
    std::vector necessaryExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME, //为了支持交换链
        VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME, //为了支持推送描述符，简化描述符集创建
        VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME, //为了支持实时调整多重采样，简化管线创建
    };
    //需打开的扩展特征
    void* necessaryFeaturesEXT;
    {
        //启用动态渲染特征
        VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures = {};
        dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
        necessaryFeaturesEXT = &dynamicRenderingFeatures;
        //启用动态多重采样状态
        VkPhysicalDeviceExtendedDynamicState3FeaturesEXT dynamicState3Features = {};
        dynamicState3Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_3_FEATURES_EXT;
        dynamicState3Features.extendedDynamicState3RasterizationSamples = VK_TRUE;
        dynamicRenderingFeatures.pNext = &dynamicState3Features;
    }


    //挑选首个满足需求的显卡
    std::vector<uint32_t> queueFamilyIndices;
    for (const auto& device : devices)
    {
        //显卡需要支持一些功能特征和功能扩展
        if (CheckDeviceFeaturesSupport(device, necessaryFeatures) == false || CheckDeviceExtensionSupport(device, necessaryExtensions) == false)
            continue;
        //显卡需要支持指定标志的队列
        queueFamilyIndices = FindQueueFamilies(device, queueFlags);
        if (queueFamilyIndices.size() != queueFlags.size())
            continue;
        //显卡需要支持呈现管道
        std::optional<uint32_t> presetQueueFamily = FindPresetQueueFamily(device, glSurface.surface);
        if (presetQueueFamily.has_value() == false)
            continue;
        queueFamilyIndices.push_back(presetQueueFamily.value());

        physicalDevice = device;
        break;
    }
    if (physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("没有找到任何一个有效GPU!");

    //准备队列创建信息
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set uniqueQueueFamilyIndices(queueFamilyIndices.begin(), queueFamilyIndices.end()); //满足要求的队列族可能是同一个，故去重
    for (uint32_t queueFamilyIndex : uniqueQueueFamilyIndices)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
        queueCreateInfo.queueCount = 1; //单线程应用，故共用一个队列即可
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    //创建逻辑设备
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    //显卡需要启用的功能特征
    createInfo.pEnabledFeatures = &necessaryFeatures;
    createInfo.pNext = necessaryFeaturesEXT;
    //显卡需要启用的功能扩展
    createInfo.enabledExtensionCount = static_cast<uint32_t>(necessaryExtensions.size());
    createInfo.ppEnabledExtensionNames = necessaryExtensions.data();

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
        throw std::runtime_error("逻辑设备创建失败!");

    //获取创建的队列
    graphicQueueFamily = queueFamilyIndices[0];
    vkGetDeviceQueue(device, queueFamilyIndices[0], 0, &graphicQueue);
    presentQueueFamily = queueFamilyIndices[1];
    vkGetDeviceQueue(device, queueFamilyIndices[1], 0, &presentQueue);

    //获取显卡设备属性
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    //获取显卡内存支持信息
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
    //获取多重采样信息
    maxUsableSampleCount = GetMaxUsableSampleCount(deviceProperties);
}
GLDevice::~GLDevice()
{
    vkDestroyDevice(device, nullptr);
}
VkFormatProperties GLDevice::GetFormatProperties(const VkFormat format) const
{
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProperties);
    return formatProperties;
}

uint32_t GLDevice::FindMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties) const
{
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & 1 << i) != 0 && (deviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("无法匹配到符合要求的内存类型！");
}
VkFormat GLDevice::FindImageFormat(const std::vector<VkFormat>& candidates, const VkImageTiling tiling, const VkFormatFeatureFlags features) const
{
    for (const VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

        //根据使用方式的不同，格式支持的功能特征也不一样
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;
        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    throw std::runtime_error("无法匹配到符合要求的纹理格式!");
}
