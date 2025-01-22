#pragma once
#include "LightMath/Runtime/Geometry/Bounds.h"
#include "LightRendering/Runtime/Resource/Material.h"
#include "LightRendering/Runtime/Resource/Mesh.h"

namespace Light
{
    struct Renderer
    {
        Material* material;
        Mesh* mesh;
        Bounds bounds;
    };
}