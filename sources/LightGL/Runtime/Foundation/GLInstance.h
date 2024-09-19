#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

class GLInstance
{
public:
    VkInstance instance = VK_NULL_HANDLE;

    explicit GLInstance(const std::vector<const char*>& validationLayers = {});
    GLInstance(const GLInstance&) = delete;
    ~GLInstance();
};
