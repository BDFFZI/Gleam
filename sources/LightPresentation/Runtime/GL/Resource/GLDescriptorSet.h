#pragma once
#include <vulkan/vulkan_core.h>

#include "GLBuffer.h"
#include "GLDescriptorPool.h"
#include "GLImageSampler.h"
#include "GLImageView.h"

/**
 * 描述符集是一个资源集合，其资源被分配在多个插槽中，这些插槽支持缓冲区，纹理等描述符类型。
 * 描述符集用于向着色器传递自定义数据，理论上每次管线运行只需设置一个描述符集即可。
 */
class GLDescriptorSet
{
public:
    VkDescriptorSet descriptorSet;

    GLDescriptorSet(const GLDescriptorPool& glDescriptorPool, const GLDescriptorSetLayout& glDescriptorSetLayout);
    GLDescriptorSet(const GLDescriptorSet&) = delete;
    ~GLDescriptorSet();

    void BindBuffer(int slotIndex, const GLBuffer& glBuffer) const;
    void BindImage(int slotIndex, const GLImageView& glImageView, const GLImageSampler& glImageSampler) const;

private:
    const GLDescriptorSetLayout& glDescriptorSetLayout;
};
