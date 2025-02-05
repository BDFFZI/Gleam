#pragma once
#include "LightMath/Runtime/Geometry/Bounds.h"
#include "LightRendering/Runtime/Asset/Material.h"
#include "LightRendering/Runtime/Asset/Mesh.h"

namespace Light
{
    struct Renderer
    {
        Material* material;
        Mesh* mesh;
        Bounds bounds;
    };
}