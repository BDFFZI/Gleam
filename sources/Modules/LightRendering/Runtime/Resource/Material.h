#pragma once
#include "LightGraphics/Runtime/GMaterial.h"

namespace Light
{
    enum class RenderQueue
    {
        Background = 1000,
        Geometry = 2000,
        AlphaTest = 2450,
        GeometryLast = 2500,
        Transparent = 3000,
        Overlay = 4000,
    };

    class Material : public GMaterial
    {
    public:
        explicit Material(ShaderAsset& shader)
            : GMaterial(shader)
        {
        }

        float renderQueue;
    };
}