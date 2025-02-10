#pragma once
#include "GleamGraphics/Runtime/Resource/GMaterial.h"

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
        explicit Material(GShader& shader)
            : GMaterial(shader)
        {
        }

        int GetRenderQueue() const;
        void SetRenderQueue(int renderQueue);

    private:
        int renderQueue = RenderQueue_Opaque;
    };
}