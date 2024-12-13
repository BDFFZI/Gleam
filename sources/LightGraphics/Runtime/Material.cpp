#include "Material.h"

#include <stdexcept>

using namespace Light;

Material::Material(ShaderAsset& shader)
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
    }

    const std::vector<VkPushConstantRange>& pushConstantRanges = *shaderAsset->glPushConstantRanges;
    glPushConstants.resize(pushConstantRanges.size());
    for (size_t i = 0; i < pushConstantRanges.size(); i++)
        glPushConstants[i].resize(pushConstantRanges[i].size);
}
Material::~Material()
{
    for (auto& descriptor : glDescriptorSets)
    {
        if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            delete descriptor.pBufferInfo;
        else if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            delete descriptor.pImageInfo;
    }
}

void Material::SetBuffer(const int slotIndex, const Buffer& buffer) const
{
    VkDescriptorBufferInfo* bufferInfo = const_cast<VkDescriptorBufferInfo*>(glDescriptorSets[slotIndex].pBufferInfo);
    bufferInfo->buffer = buffer.GetGLBuffer().buffer;
    bufferInfo->offset = 0;
    bufferInfo->range = buffer.GetGLBuffer().size;
    //TODO 需要dirty？
}
void Material::SetTexture(const int slotIndex, const TextureAsset& texture) const
{
    VkDescriptorImageInfo* imageInfo = const_cast<VkDescriptorImageInfo*>(glDescriptorSets[slotIndex].pImageInfo);
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo->imageView = texture.glImageView->imageView;
    imageInfo->sampler = texture.glImageSampler->imageSampler;
    //TODO 需要dirty？
}
void Material::SetPushConstant(const int slotIndex, const void* data)
{
    std::memcpy(glPushConstants[slotIndex].data(), data, (*shaderAsset->glPushConstantRanges)[slotIndex].size);
    isDirty = true;
}
void Material::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialAsset* lastMaterial)
{
    if (this != lastMaterial || isDirty)
    {
        MaterialAsset::BindToPipeline(glCommandBuffer, lastMaterial);
        isDirty = false;
    }
}
