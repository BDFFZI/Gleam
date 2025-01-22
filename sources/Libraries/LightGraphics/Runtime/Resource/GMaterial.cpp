#include "GMaterial.h"

#include <stdexcept>

namespace Light
{
    GMaterial::GMaterial(GSAssetLayout& assetLayout)
        : assetLayout(&assetLayout), isDirty(true)
    {
        //创建描述符上传结构
        const std::vector<GLDescriptorBinding>& descriptorBindings = assetLayout.GetDescriptorBindings();
        descriptorSetsUpload.resize(descriptorBindings.size());
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

            if (descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) //缓冲区资源
                writeDescriptorSet.pBufferInfo = new VkDescriptorBufferInfo();
            else if (descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) //纹理资源
                writeDescriptorSet.pImageInfo = new VkDescriptorImageInfo();
            else
                throw std::runtime_error("不支持的描述符类型");

            descriptorSetsUpload[i] = writeDescriptorSet;
        }

        //创建推送常量上传结构
        const std::vector<VkPushConstantRange>& pushConstantRanges = assetLayout.GetPushConstantRanges();
        pushConstantsUpload.resize(pushConstantRanges.size());
        for (size_t i = 0; i < pushConstantRanges.size(); i++)
            pushConstantsUpload[i].resize(pushConstantRanges[i].size);

        //为部分资源设置默认值
        for (size_t i = 0; i < descriptorSetsUpload.size(); i++)
        {
            if (descriptorSetsUpload[i].descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                SetTexture(static_cast<int>(i), Graphics::GetDefaultTexture2D());
        }
    }
    GMaterial::GMaterial(GShader& pass)
        : GMaterial()
    {
        AddPass("", pass);
    }

    GMaterial::~GMaterial()
    {
        for (auto& descriptor : descriptorSetsUpload)
        {
            if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
                delete descriptor.pBufferInfo;
            else if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                delete descriptor.pImageInfo;
        }
    }

    void GMaterial::SetBuffer(const int slotIndex, const GBuffer& buffer)
    {
        VkDescriptorBufferInfo* bufferInfo = const_cast<VkDescriptorBufferInfo*>(descriptorSetsUpload[slotIndex].pBufferInfo);
        bufferInfo->buffer = buffer.GetGLBuffer().buffer;
        bufferInfo->offset = 0;
        bufferInfo->range = buffer.GetGLBuffer().size;
        isDirty = true;
    }
    void GMaterial::SetTexture(const int slotIndex, const GTexture& texture)
    {
        VkDescriptorImageInfo* imageInfo = const_cast<VkDescriptorImageInfo*>(descriptorSetsUpload[slotIndex].pImageInfo);
        imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo->imageView = texture.GetGLImageView().imageView;
        imageInfo->sampler = texture.GetGLImageSampler().imageSampler;
        isDirty = true;
    }
    void GMaterial::SetPushConstant(const int slotIndex, const void* data)
    {
        std::memcpy(pushConstantsUpload[slotIndex].data(), data, assetLayout->GetPushConstantRanges()[slotIndex].size);
        isDirty = true;
    }
    void GMaterial::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GMaterial* lastMaterial)
    {
        if (this != lastMaterial || isDirty)
        {
            //上传描述符
            glCommandBuffer.PushDescriptorSetKHR(assetLayout->GetGLPipelineLayout(), descriptorSetsUpload);
            //上传推送常量
            const std::vector<VkPushConstantRange>& pushConstantRanges = assetLayout->GetPushConstantRanges();
            for (size_t i = 0; i < pushConstantRanges.size(); i++)
            {
                glCommandBuffer.PushConstant(
                    assetLayout->GetGLPipelineLayout(),
                    pushConstantRanges[i],
                    pushConstantsUpload[i].data());
            }
            isDirty = false;
        }
    }
    void GMaterial::AddPass(const std::string& passName, GShader& shader)
    {
        passes.emplace_back(passName, &shader);
    }
    const std::vector<std::tuple<std::string, GShader*>>& GMaterial::GetPasses()
    {
        return passes;
    }
}