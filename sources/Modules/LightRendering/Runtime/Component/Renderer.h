#pragma once
#include "LightGraphics/Runtime/Mesh.h"
#include "LightMath/Runtime/Geometry/Bounds.h"
#include "LightRendering/Runtime/Resource/Material.h"

namespace Light
{
    struct Renderer
    {
        Material* material;
        Mesh* mesh;
        Bounds bounds;
    };
}