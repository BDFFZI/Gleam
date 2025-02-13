#pragma once
#include "GleamRendering/Runtime/Asset/Material.h"
#include "GleamRendering/Runtime/Asset/Mesh.h"

namespace Gleam
{
    struct Renderer
    {
        std::optional<Material*> material;
        std::optional<Mesh*> mesh;
    };
}