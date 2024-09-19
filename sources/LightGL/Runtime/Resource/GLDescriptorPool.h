#pragma once

#include <vulkan/vulkan_core.h>
#include "GLDescriptorSetLayout.h"

class GLDescriptorPool
{
public:
    VkDescriptorPool descriptorPool;

    GLDescriptorPool(const GLDescriptorSetLayout& descriptorSetLayout, int descriptorSetCount);
    GLDescriptorPool(const GLDescriptorPool&) = delete;
    ~GLDescriptorPool();
};
