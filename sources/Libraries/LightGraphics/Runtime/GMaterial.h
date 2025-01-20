#pragma once
#include "Buffer.h"
#include "GraphicsAssets.h"

namespace Light
{
    class GMaterial : public MaterialAsset
    {
    public:
        GMaterial(ShaderAsset& shader);
        GMaterial(const GMaterial&) = delete;
        ~GMaterial() override;

        void SetBuffer(int slotIndex, const Buffer& buffer);
        void SetTexture(int slotIndex, const TextureAsset& texture);
        void SetPushConstant(int slotIndex, const void* data);

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialAsset* lastMaterial) override;

    protected:
        bool isDirty;
    };
}
