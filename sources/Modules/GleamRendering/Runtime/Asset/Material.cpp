#include "Material.h"

namespace Gleam
{
    int Material::GetRenderQueue() const
    {
        return renderQueue;
    }
    void Material::SetRenderQueue(const int renderQueue)
    {
        this->renderQueue = renderQueue;
    }
    void Material::SetMaterialInfo(const MaterialInfo& materialInfo) const
    {
        materialInfoBuffer.SetData(&materialInfo);
    }
    void Material::SetAlbedoTex(GTexture& albedoTex)
    {
        SetTexture(2, albedoTex);
    }
    void Material::SetMetallicGlossTex(GTexture& metallicGlossTex)
    {
        SetTexture(3, metallicGlossTex);
    }
    void Material::SetNormalTex(GTexture& normalTex)
    {
        SetTexture(4, normalTex);
    }
}