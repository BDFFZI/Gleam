#pragma once
#include "GLDevice.h"

class GLCommandPool
{
public:
    VkCommandPool commandPool;

    explicit GLCommandPool(const GLDevice& glDevice);
    GLCommandPool(const GLCommandPool&) = delete;
    ~GLCommandPool();
private:
    VkDevice device;
};
