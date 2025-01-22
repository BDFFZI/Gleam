#pragma once
#include "LightGraphics/Runtime/Resource/GMaterial.h"

namespace Light
{
    enum RenderQueue:uint16_t
    {
        RenderQueue_Shadow = 1000,
        RenderQueue_Opaque = 2000,
        RenderQueue_Skybox = 3000,
        RenderQueue_Transparent = 4000,
        RenderQueue_PostProcessing = 5000,
    };

    inline static const std::vector<GLDescriptorBinding> DefaultDescriptorBindings = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, //反照率与透明度贴图
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, //金属度与光泽度贴图
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} //法线贴图
    };


    class Material : public GMaterial
    {
    public:
        explicit Material(GShader& shader)
            : GMaterial(shader)
        {
        }

        int GetRenderQueue() const { return renderQueue; }

    private:
        int renderQueue = RenderQueue_Opaque;
    };
}