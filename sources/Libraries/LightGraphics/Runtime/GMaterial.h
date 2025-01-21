#pragma once
#include "GBuffer.h"
#include "GraphicsAssets.h"

namespace Light
{
    class GMaterial
    {
    public:
        GMaterial(GShader& shader);
        GMaterial(const GMaterial&) = delete;
        virtual ~GMaterial();

        void SetBuffer(int slotIndex, const GBuffer& buffer);
        void SetTexture(int slotIndex, const TextureAsset& texture);
        void SetPushConstant(int slotIndex, const void* data);

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GMaterial* lastMaterial) override;

    protected:
        GShader* shaderAsset;
        std::vector<VkWriteDescriptorSet> glDescriptorSets;
        std::vector<std::vector<std::byte>> glPushConstants;
        bool isDirty;
    };
}