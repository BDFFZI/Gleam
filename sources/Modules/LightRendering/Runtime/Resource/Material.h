#pragma once
#include "LightGraphics/Runtime/GMaterial.h"

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