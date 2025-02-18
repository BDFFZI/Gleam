#pragma once
#include "GleamGraphics/Runtime/Resource/GMaterial.h"
#include "GleamRendering/Runtime/RenderingConfig.h"

namespace Gleam
{
    enum RenderQueue:uint16_t
    {
        RenderQueue_Shadow = 1000,
        RenderQueue_Opaque = 2000,
        RenderQueue_Skybox = 3000,
        RenderQueue_Transparent = 4000,
        RenderQueue_PostProcessing = 5000,
    };

    class Material : public GMaterial
    {
    public:
        explicit Material(GShader& shader, GSAssetLayout& assetLayout = Graphics::GetGraphicsConfig().defaultGSAssetLayout)
            : GMaterial(assetLayout)
        {
            AddPass("", shader);
            SetBuffer(1, materialInfoBuffer);
            SetMaterialInfo(MaterialInfo{});
        }

        int GetRenderQueue() const;
        void SetRenderQueue(int renderQueue);

        void SetMaterialInfo(const MaterialInfo& materialInfo) const;
        void SetAlbedoTex(GTexture& albedoTex);
        void SetMetallicGlossTex(GTexture& metallicGlossTex);
        void SetNormalTex(GTexture& normalTex);

    private:
        int renderQueue = RenderQueue_Opaque;
        GBuffer materialInfoBuffer = GBuffer{sizeof(MaterialInfo)};
    };
}