﻿#pragma once
#include "GBuffer.h"
#include "GShader/GShader.h"
#include "GShader/GSLayout.h"

namespace Gleam
{
    class GMaterial
    {
    public:
        GMaterial(GSAssetLayout& assetLayout = Graphics::GetGraphicsConfig().defaultGSAssetLayout);
        GMaterial(const GShader& pass);
        virtual ~GMaterial();

        GMaterial(const GMaterial&) = delete;
        GMaterial& operator=(const GMaterial&) = delete;
        GMaterial(GMaterial&&) = default;
        GMaterial& operator=(GMaterial&&) = default;

        void SetBuffer(int slotIndex, const GBuffer& buffer);
        void SetTexture(int slotIndex, const GTexture& texture);
        void SetPushConstant(int slotIndex, const void* data);
        void BindToPipeline(const GLCommandBuffer& glCommandBuffer, const GMaterial* lastMaterial);

        void AddPass(const std::string& passName, const GShader& shader);
        const std::vector<std::tuple<std::string, const GShader*>>& GetPasses();

    protected:
        GSAssetLayout* assetLayout;
        std::vector<VkWriteDescriptorSet> descriptorSetsUpload;
        std::vector<std::vector<std::byte>> pushConstantsUpload;
        std::vector<std::tuple<std::string, const GShader*>> passes;
        bool isDirty;
    };
}