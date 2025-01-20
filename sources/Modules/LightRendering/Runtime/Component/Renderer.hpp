#pragma once
#include "LightMath/Runtime/Geometry/Bounds.hpp"
#include "LightGraphics/Runtime/GraphicsAssets.h"

namespace Light
{
    struct Renderer
    {
        MaterialAsset* material;
        Bounds bounds;
    };
}