#include "GraphicsAssets.h"

namespace Light
{
    void MeshAsset::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MeshAsset* currentMesh)
    {
        glCommandBuffer.BindVertexBuffers(*glVertexBuffer);
        glCommandBuffer.BindIndexBuffer(*glIndexBuffer);
    }
    void ShaderAsset::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const ShaderAsset* currentShader)
    {
        glCommandBuffer.BindPipeline(*glPipeline);
    }
    void MaterialAsset::BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialAsset* currentMaterial)
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
    }
}
