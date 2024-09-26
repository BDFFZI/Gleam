#include "Material.h"

#include <stdexcept>

using namespace LightRuntime;

Material::Material(Shader& shader): shader(&shader)
{
    const std::vector<GLDescriptorBinding>& descriptorBindings = shader.GetGLDescriptorSetLayout().descriptorBindings;
    descriptorSet.resize(descriptorBindings.size());
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

        descriptorSet[i] = writeDescriptorSet;
    }
}
Material::~Material()
{
    for (auto& descriptor : descriptorSet)
    {
        if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            delete descriptor.pBufferInfo;
        else if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            delete descriptor.pImageInfo;
    }
}

const Shader& Material::GetShader() const
{
    return *shader;
}
const std::vector<VkWriteDescriptorSet>& Material::GetDescriptorSet() const
{
    return descriptorSet;
}

void Material::SetBuffer(const int slotIndex, const Buffer& buffer) const
{
    VkDescriptorBufferInfo* bufferInfo = const_cast<VkDescriptorBufferInfo*>(descriptorSet[slotIndex].pBufferInfo);
    bufferInfo->buffer = buffer.GetGLBuffer().buffer;
    bufferInfo->offset = 0;
    bufferInfo->range = buffer.GetGLBuffer().size;
}
void Material::SetTexture2D(const int slotIndex, const Texture2D& texture2D) const
{
    VkDescriptorImageInfo* imageInfo = const_cast<VkDescriptorImageInfo*>(descriptorSet[slotIndex].pImageInfo);
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo->imageView = texture2D.GetGLImageView().imageView;
    imageInfo->sampler = texture2D.GetGLImageSampler().imageSampler;
}
