#pragma once
#include "GLInstance.h"
#include "GLSurface.h"

/**
 * 主要用于设备信息的查询，对于具体的设备操作则交由后续的各部件自行调用实现
 */
class GLDevice
{
public:
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t graphicQueueFamily;
    VkQueue graphicQueue;
    uint32_t presentQueueFamily;
    VkQueue presentQueue;
    VkPhysicalDeviceProperties deviceProperties; //!< 显卡各种的属性信息，包括功能限制
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties; //!< 显卡支持分配的内存种类
    VkSampleCountFlagBits maxUsableSampleCount; //!< 最大多重采样次数

    GLDevice(const GLInstance& glInstance, const GLSurface& glSurface);
    GLDevice(const GLDevice&) = delete;
    ~GLDevice();

    VkFormatProperties GetFormatProperties(VkFormat format) const;

    /**
     * 在指定的内存类型范围中搜索首个满足属性要求的内存类型编号
     * @param typeFilter 内存类型范围
     * @param properties 属性要求
     * @return 
     */
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
    /**
     * 在指定的纹理格式中寻找首个满足特征要求的纹理格式
     * @param candidates 指定的纹理格式
     * @param tiling 该格式将用于的像素布局
     * @param features 需要支持的功能特征
     * @return 
     */
    VkFormat FindImageFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
};
