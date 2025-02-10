#pragma once
#include "Renderer.h"
#include "GleamMath/Runtime/Geometry/Segment.h"

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