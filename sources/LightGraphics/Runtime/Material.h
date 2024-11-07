#pragma once
#include "Buffer.h"
#include "Shader.h"
#include "Texture2D.h"

namespace Light
{
    class Material : public MaterialBase
    {
    public:
        Material(ShaderBase* shader);
        Material(const Material&) = delete;
        ~Material() override;

        const ShaderBase* GetShader() const override { return shader; }
        void SetBuffer(int slotIndex, const Buffer& buffer) const;
        void SetTexture2D(int slotIndex, const Texture2D& texture2D) const;

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialBase* lastMaterial) override;

    private:
        ShaderBase* shader;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
    };
}
