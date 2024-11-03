#pragma once
#include "Buffer.h"
#include "Shader.h"
#include "Texture2D.h"
#include "LightGL/Runtime/Resource/GLDescriptorSet.h"

namespace Light
{
    class Material
    {
    public:
        Material(Shader& shader);
        Material(const Material&) = delete;
        ~Material();

        const Shader& GetShader() const;
        const std::vector<VkWriteDescriptorSet>& GetDescriptorSet() const;

        void SetBuffer(int slotIndex, const Buffer& buffer) const;
        void SetTexture2D(int slotIndex, const Texture2D& texture2D) const;

    private:
        Shader* shader;
        std::vector<VkWriteDescriptorSet> descriptorSet;
    };
}
