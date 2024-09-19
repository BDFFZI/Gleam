#pragma once
#include <vulkan/vulkan_core.h>

class GLImageSampler
{
public:
    VkSampler imageSampler;

    GLImageSampler();
    GLImageSampler(const GLImageSampler&) = delete;
    ~GLImageSampler();
};
