#include "GLInstance.h"

#include <stdexcept>
#include <GLFW/glfw3.h>


bool GLInstance::CheckValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    //获取支持的所有Vulkan层
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    //确定所需的层都在支持列表内
    for (const char* layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

GLInstance::GLInstance(const std::vector<const char*>& validationLayers)
{
    //使用Vulkan的应用程序信息
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = nullptr;
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = nullptr;
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3; //VK_KHR_dynamic_rendering需要

    //要启用的Vulkan扩展
    uint32_t glfwExtensionCount;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); //Vulkan完全与系统无关，所以窗口系统集成是扩展功能
    std::vector<const char*> extensions(glfwExtensionCount);
    memcpy(reinterpret_cast<void*>(extensions.data()), reinterpret_cast<const void*>(glfwExtensions),
           sizeof(const char*) * glfwExtensionCount);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    //是否启用验证层（验证层就是在vulkan代码中加一层异常判断，牺牲效率来方便调试）
    if (!validationLayers.empty())
    {
#ifdef _DEBUG
        if (!CheckValidationLayerSupport(validationLayers))
            throw std::runtime_error("不支持验证层！");
#endif

        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
    }

    if (VkResult result = vkCreateInstance(&createInfo, nullptr, &instance); result != VK_SUCCESS)
        throw std::runtime_error("创建Vulkan实例失败！");
}

GLInstance::~GLInstance()
{
    vkDestroyInstance(instance, nullptr);
}
