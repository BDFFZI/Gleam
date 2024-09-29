#include "GLImage.h"

#include <array>
#include <stdexcept>

#include "GLBuffer.h"
#include "../Pipeline/GLCommandBuffer.h"
#include "../GL.h"

void CmdTransitionImageLayout(
    const GLCommandBuffer& glCommandBuffer, const VkImage& image,
    const VkImageLayout oldLayout, const VkImageLayout newLayout,
    const uint32_t mipmapBasis, const uint32_t mipmapCount,
    const VkAccessFlags srcAccessMask, const VkAccessFlags dstAccessMask,
    const VkPipelineStageFlags srcStage, const VkPipelineStageFlags dstStage)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    //转换图形内存布局
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    //不使用队列所有权转移功能
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    //受影响的图形
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; //视作颜色纹理
    barrier.subresourceRange.levelCount = mipmapCount; //转换mipmap的数量
    barrier.subresourceRange.baseMipLevel = mipmapBasis; //转换mipmap的起点
    barrier.subresourceRange.layerCount = 1; //非数组
    barrier.subresourceRange.baseArrayLayer = 0;
    //屏障执行前后需要等待的操作类型
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;

    vkCmdPipelineBarrier(
        glCommandBuffer.commandBuffer,
        srcStage, //屏障要在哪个阶段后开始
        dstStage, //屏障要在哪个阶段前完成
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );
}
void CmdBlitImageMipmap(
    const GLCommandBuffer& glCommandBuffer, const VkImage& image,
    const uint32_t sourceMipLevel, const VkOffset2D& sourceRegion,
    const uint32_t destinationMipLevel, const VkOffset2D& destinationRegion)
{
    VkImageBlit blit{};
    blit.srcOffsets[0] = {0, 0, 0};
    blit.srcOffsets[1] = {sourceRegion.x, sourceRegion.y, 1};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = sourceMipLevel;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {0, 0, 0};
    blit.dstOffsets[1] = {destinationRegion.x, destinationRegion.y, 1};
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = destinationMipLevel;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(glCommandBuffer.commandBuffer,
                   image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1, &blit,
                   VK_FILTER_LINEAR);
}

GLImage* GLImage::CreateTexture2D(const uint32_t width, const uint32_t height, const void* data, const size_t size, const bool mipChain)
{
    const uint32_t mipLevels = mipChain ? static_cast<uint32_t>(std::floor(std::log2(std::max(width, height))) + 1) : 1;
    if (mipLevels != 1)
    {
        VkFormatProperties formatProperties = GL::glDevice->GetFormatProperties(VK_FORMAT_R8G8B8A8_SRGB);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
            throw std::runtime_error("该纹理格式不支持线性过滤，故无法自动生成mipmap！");
    }

    GLImage* glImage = new GLImage(
        width, height,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        mipLevels
    );

    std::unique_ptr<GLBuffer> transmitter(GLBuffer::CreateTransmitter(data, size));
    GLCommandBuffer::ExecuteSingleTimeCommands([&](const GLCommandBuffer& commandBuffer)
    {
        //图片整体转为传输目标布局
        CmdTransitionImageLayout(
            commandBuffer, glImage->image,
            glImage->layout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, mipLevels,
            0, //屏障不需要等到特定操作后执行
            VK_ACCESS_TRANSFER_WRITE_BIT, //传输（写入）操作必须在屏障完成后
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, //屏障在图形管道的最早阶段就可执行
            VK_PIPELINE_STAGE_TRANSFER_BIT //必须确保屏障在传输阶段前完成
        );
        //填充一级mipmap
        commandBuffer.CopyBufferToImage(*transmitter, *glImage);

        //遍历后续mipmap（如果没有生成mipmap则不会触发）
        int32_t mipWidth = static_cast<int32_t>(width);
        int32_t mipHeight = static_cast<int32_t>(height);
        for (uint32_t i = 1; i < mipLevels; i++)
        {
            //将前一级mipmap转为传输源
            CmdTransitionImageLayout(
                commandBuffer, glImage->image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, i - 1, 1,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT
            );
            //将前一级mipmap内容传输到当前级
            CmdBlitImageMipmap(
                commandBuffer, glImage->image,
                i - 1, {mipWidth, mipHeight},
                i, {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1}
            );
            //前一级mipmap使用完毕，将其转为着色器只读模式
            CmdTransitionImageLayout(
                commandBuffer, glImage->image,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, i - 1, 1,
                VK_ACCESS_TRANSFER_READ_BIT, //传输完成后才可执行该屏障
                VK_ACCESS_SHADER_READ_BIT, //该屏障完成后着色器才可进行资源读取操作
                VK_PIPELINE_STAGE_TRANSFER_BIT, //发生在传输阶段
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT //发生在片段着色器执行之前
            );

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        //处理最后一个mipmap的转换
        CmdTransitionImageLayout(
            commandBuffer, glImage->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels - 1, 1,
            VK_ACCESS_TRANSFER_WRITE_BIT, //传输完成后才可执行该屏障
            VK_ACCESS_SHADER_READ_BIT, //该屏障完成后着色器才可进行资源读取操作
            VK_PIPELINE_STAGE_TRANSFER_BIT, //发生在传输阶段
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT //发生在片段着色器执行之前
        );
    });


    glImage->layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    return glImage;
}
GLImage* GLImage::CreateFrameBufferColor(const uint32_t width, const uint32_t height, const VkFormat colorFormat, const VkSampleCountFlagBits sampleCount)
{
    return new GLImage(width, height, colorFormat,
                       VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT, 1, sampleCount);
}
GLImage* GLImage::CreateFrameBufferDepth(const uint32_t width, const uint32_t height, const VkFormat depthFormat, const VkSampleCountFlagBits sampleCount)
{
    return new GLImage(width, height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 1, sampleCount);
}

GLImage::GLImage(const uint32_t width, const uint32_t height, const VkFormat format, const VkImageUsageFlags usageFlags,
                 const uint32_t mipLevels, const VkSampleCountFlagBits sampleCount)
    : layout(VK_IMAGE_LAYOUT_UNDEFINED), format(format), width(width), height(height), mipLevels(mipLevels)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D; //普通2D纹理
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1; //非3D纹理无深度
    imageInfo.arrayLayers = 1; //非纹理数组无层次
    imageInfo.mipLevels = mipLevels;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL; //像素布局方式，人类易用的方式是线性按行布局，但这种方式对图像处理并不友好（上下像素内存地址过远），当不关心内容的人类可读性时应让系统自行选择最优布局。
    imageInfo.initialLayout = layout;
    imageInfo.usage = usageFlags;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //图形队列独占
    imageInfo.samples = sampleCount; //多重采样
    imageInfo.flags = 0; // Optional
    if (vkCreateImage(GL::glDevice->device, &imageInfo, nullptr, &image) != VK_SUCCESS)
        throw std::runtime_error("创建图片失败！");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(GL::glDevice->device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    //VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT是GPU访问最高效的内存属性，但通常也无法被CPU直接访问
    allocInfo.memoryTypeIndex = GL::glDevice->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(GL::glDevice->device, &allocInfo, nullptr, &memory) != VK_SUCCESS)
        throw std::runtime_error("分配图片内存失败！");

    vkBindImageMemory(GL::glDevice->device, image, memory, 0);
}
GLImage::~GLImage()
{
    vkDestroyImage(GL::glDevice->device, image, nullptr);
    vkFreeMemory(GL::glDevice->device, memory, nullptr);
}
