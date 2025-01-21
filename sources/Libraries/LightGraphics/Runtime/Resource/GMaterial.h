#pragma once
#include "GBuffer.h"
#include "GShader/GShader.h"
#include "GShader/GShaderLayout.h"

namespace Light
{
    class GMaterial
    {
    public:
        GMaterial(GShaderLayout& shaderLayout, GShader* shader = nullptr);
        GMaterial(const GMaterial&) = delete;
        virtual ~GMaterial();

        void SetBuffer(int slotIndex, const GBuffer& buffer);
        void SetTexture(int slotIndex, const GTexture& texture);
        void SetPushConstant(int slotIndex, const void* data);
        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GMaterial* lastMaterial);

        void AddPass(const std::string& passName, GShader& shader);
        const std::vector<std::tuple<std::string, GShader*>>& GetPasses();

    protected:
        GShaderLayout* shaderLayout;
        std::vector<VkWriteDescriptorSet> descriptorSetsUpload;
        std::vector<std::vector<std::byte>> pushConstantsUpload;
        std::vector<std::tuple<std::string, GShader*>> shaderPasses;
        bool isDirty;
    };
}