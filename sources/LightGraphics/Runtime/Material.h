#pragma once
#include "Buffer.h"
#include "GraphicsAssets.h"

namespace Light
{
    class Material : public MaterialAsset
    {
    public:
        Material(ShaderAsset& shader);
        Material(const Material&) = delete;
        ~Material() override;

        void SetBuffer(int slotIndex, const Buffer& buffer) const;
        void SetTexture(int slotIndex, const TextureAsset& texture) const;
        void SetPushConstant(int slotIndex, const void* data);

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialAsset* lastMaterial) override;

    protected:
        bool isDirty;
    };
}
