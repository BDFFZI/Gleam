#pragma once
#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"

namespace Light
{
    class MaterialBase
    {
    public:
        virtual ~MaterialBase() = default;
        virtual void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialBase* lastMaterial) = 0;
        virtual const ShaderBase* GetShader() const = 0;
    };
    
    class MaterialT : public MaterialBase
    {
    public:
        MaterialT(ShaderBase& shader);
        MaterialT(const MaterialT&) = delete;
        ~MaterialT() override;

        const ShaderBase* GetShader() const override { return shader; }
        void SetBuffer(int slotIndex, const Buffer& buffer) const;
        void SetTexture(int slotIndex, const TextureBase& texture) const;

        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const MaterialBase* lastMaterial) override;

    private:
        ShaderBase* shader;
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;
    };
}
