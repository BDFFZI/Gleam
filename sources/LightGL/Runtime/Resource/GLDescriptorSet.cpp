#include "GLDescriptorSet.h"

#include <stdexcept>

#include "../GL.h"

GLDescriptorSet::GLDescriptorSet(const GLDescriptorPool& glDescriptorPool, const GLDescriptorSetLayout& glDescriptorSetLayout)
    : glDescriptorSetLayout(glDescriptorSetLayout)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = glDescriptorPool.descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &glDescriptorSetLayout.descriptorSetLayout;

    if (vkAllocateDescriptorSets(GL::glDevice->device, &allocInfo, &descriptorSet) != VK_SUCCESS)
        throw std::runtime_error("创建描述符集失败！");
}
GLDescriptorSet::~GLDescriptorSet() = default;


void GLDescriptorSet::BindBuffer(const int slotIndex, const GLBuffer& glBuffer) const
{
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = glBuffer.buffer;
    bufferInfo.offset = 0;
    bufferInfo.range = glBuffer.size;

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.dstBinding = slotIndex;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = glDescriptorSetLayout.descriptorBindings[slotIndex].descriptorType;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(GL::glDevice->device, 1, &writeDescriptorSet, 0, nullptr);
}
void GLDescriptorSet::BindImage(const int slotIndex, const GLImageView& glImageView, const GLImageSampler& glImageSampler) const
{
    VkDescriptorImageInfo imageInfo;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = glImageView.imageView;
    imageInfo.sampler = glImageSampler.imageSampler;

    VkWriteDescriptorSet writeDescriptorSet{};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.dstBinding = slotIndex;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = glDescriptorSetLayout.descriptorBindings[slotIndex].descriptorType;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(GL::glDevice->device, 1, &writeDescriptorSet, 0, nullptr);
}
