#pragma once
#include "GleamRendering/Runtime/Asset/Material.h"
#include "GleamRendering/Runtime/Asset/Mesh.h"

namespace Gleam
{
    struct Renderer
    {
        std::weak_ptr<Material> material;
        std::weak_ptr<Mesh> mesh;
    };
    Gleam_MakeType(Renderer, "477A18F2-8A44-4051-BEEE-D0A2D77CF36B")
    {
        Gleam_MakeType_AddField(material);
        Gleam_MakeType_AddField(mesh);
    }
}