#pragma once
#include <vulkan/vulkan_core.h>

#include "GLImage.h"

class GLImageView
{
public:
    VkImageView imageView;
    VkFormat format;

    GLImageView(VkImage image, VkFormat format, const VkImageAspectFlags& aspectFlags, uint32_t mipLevels = 1);
    GLImageView(const GLImage& glImage, const VkImageAspectFlags& aspectFlags);
    GLImageView(const GLImageView&) = delete;
    ~GLImageView();
};
