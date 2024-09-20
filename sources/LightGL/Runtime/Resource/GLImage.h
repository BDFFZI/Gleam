#pragma once
#include <vulkan/vulkan_core.h>

class GLImage
{
public:
    /**
     * 支持作为传输目标（解决当内存为仅GPU可用时的内容初始化）和着色器采样的纹理
     * @param width 
     * @param height 
     * @param data 初始像素内容
     * @param size 初始像素内容大小
     * @param mipChain
     * @return 
     */
    static GLImage* CreateTexture2D(uint32_t width, uint32_t height, const void* data, int size, bool mipChain = false);
    static GLImage* CreateFrameBufferColor(uint32_t width, uint32_t height, VkFormat colorFormat, VkSampleCountFlagBits sampleCount);
    static GLImage* CreateFrameBufferDepth(uint32_t width, uint32_t height, VkFormat depthFormat, VkSampleCountFlagBits sampleCount);

    VkImage image;
    VkDeviceMemory memory;
    VkImageLayout layout;
    VkFormat format;
    uint32_t width;
    uint32_t height;
    uint32_t mipLevels;

    GLImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, uint32_t mipmapCount = 1, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);
    GLImage(const GLImage&) = delete;
    ~GLImage();
};
