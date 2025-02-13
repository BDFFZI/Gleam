#pragma once
#include "GleamMath/Runtime/Geometry/Solid/Segment.h"
#include "GleamRendering/Runtime/Asset/Mesh.h"

namespace Gleam
{
    struct LinesMesh
    {
        std::vector<Segment> lines;

    private:
        friend class LinesRendererSystem;
        std::optional<std::unique_ptr<Mesh>> linesMesh;
    };

    Gleam_MakeType(LinesMesh, "253F9DB5-B894-43FD-A319-A69717536850")
    {
        Gleam_MakeType_AddField(lines);
    }
}