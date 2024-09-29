#include "GLImageView.h"

#include <stdexcept>

#include "../GL.h"

GLImageView::GLImageView(const VkImage image, const VkFormat format, const VkImageAspectFlags& aspectFlags, const uint32_t mipLevels)
    : format(format)
{
    VkImageViewCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format = format;
    //不重新映射颜色通道
    createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    //映射意图和映射的图片范围
    createInfo.subresourceRange.aspectMask = aspectFlags; //映射模式，即将图片内容视为什么，如颜色或深度
    createInfo.subresourceRange.levelCount = mipLevels;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.layerCount = 1; //不使用纹理数组
    createInfo.subresourceRange.baseArrayLayer = 0;

    if (vkCreateImageView(GL::glDevice->device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("创建图片视图失败！");
}
GLImageView::GLImageView(const GLImage& glImage, const VkImageAspectFlags& aspectFlags)
    : GLImageView(glImage.image, glImage.format, aspectFlags, glImage.mipLevels)
{
}
GLImageView::~GLImageView()
{
    vkDestroyImageView(GL::glDevice->device, imageView, nullptr);
}
