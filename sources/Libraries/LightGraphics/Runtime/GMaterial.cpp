#include "GMaterial.h"

#include <stdexcept>

#include "GraphicsPreset.hpp"

using namespace Light;

GMaterial::GMaterial(GShader& shader)
    : isDirty(true)
{
    shaderAsset = &shader;

    const std::vector<GLDescriptorBinding>& descriptorBindings = *shaderAsset->glDescriptorBindings;
    glDescriptorSets.resize(descriptorBindings.size());
    for (size_t i = 0; i < descriptorBindings.size(); i++)
    {
        VkDescriptorType descriptorType = descriptorBindings[i].descriptorType;

        VkWriteDescriptorSet writeDescriptorSet{};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = VK_NULL_HANDLE;
        writeDescriptorSet.dstBinding = static_cast<uint32_t>(i);
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorType = descriptorType;
        writeDescriptorSet.descriptorCount = 1;

        if (descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            writeDescriptorSet.pBufferInfo = new VkDescriptorBufferInfo();
        else if (descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            writeDescriptorSet.pImageInfo = new VkDescriptorImageInfo();
        else
            throw std::runtime_error("不支持的描述符类型");

        glDescriptorSets[i] = writeDescriptorSet;

        if (descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            SetTexture(i, *GraphicsPreset::DefaultTexture2D);
    }

    const std::vector<VkPushConstantRange>& pushConstantRanges = *shaderAsset->glPushConstantRanges;
    glPushConstants.resize(pushConstantRanges.size());
    for (size_t i = 0; i < pushConstantRanges.size(); i++)
        glPushConstants[i].resize(pushConstantRanges[i].size);
}
GMaterial::~GMaterial()
{
    for (auto& descriptor : glDescriptorSets)
    {
        if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            delete descriptor.pBufferInfo;
        else if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            delete descriptor.pImageInfo;
    }
}

void GMaterial::SetBuffer(const int slotIndex, const GBuffer& buffer)
{
    VkDescriptorBufferInfo* bufferInfo = const_cast<VkDescriptorBufferInfo*>(glDescriptorSets[slotIndex].pBufferInfo);
    bufferInfo->buffer = buffer.GetGLBuffer().buffer;
    bufferInfo->offset = 0;
    bufferInfo->range = buffer.GetGLBuffer().size;
    isDirty = true;
}
void GMaterial::SetTexture(const int slotIndex, const TextureAsset& texture)
{
    VkDescriptorImageInfo* imageInfo = const_cast<VkDescriptorImageInfo*>(glDescriptorSets[slotIndex].pImageInfo);
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo->imageView = texture.glImageView->imageView;
    imageInfo->sampler = texture.glImageSampler->imageSampler;
    isDirty = true;
}
void GMaterial::SetPushConstant(const int slotIndex, const void* data)
{
    std::memcpy(glPushConstants[slotIndex].data(), data, (*shaderAsset->glPushConstantRanges)[slotIndex].size);
    isDirty = true;
}
void GMaterial::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GMaterial* lastMaterial)
{
    if (this != lastMaterial || isDirty)
    {
        //上传描述符
        glCommandBuffer.PushDescriptorSetKHR(*shaderAsset->glPipelineLayout, glDescriptorSets);
        //上传推送常量
        const std::vector<VkPushConstantRange>& pushConstantRanges = *shaderAsset->glPushConstantRanges;
        for (size_t i = 0; i < pushConstantRanges.size(); i++)
        {
            glCommandBuffer.PushConstant(
                *shaderAsset->glPipelineLayout,
                pushConstantRanges[i],
                glPushConstants[i].data());
        }
        isDirty = false;
    }
}
