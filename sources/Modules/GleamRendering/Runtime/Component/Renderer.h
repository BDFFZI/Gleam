#pragma once
#include "GleamMath/Runtime/Geometry/Cube.h"
#include "GleamRendering/Runtime/Asset/Material.h"
#include "GleamRendering/Runtime/Asset/Mesh.h"

namespace Gleam
{
    struct Renderer
    {
        std::optional<Material*> material;
        std::optional<Mesh*> mesh;
        Cube bounds;
    };
}