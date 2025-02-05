#pragma once
#include "Renderer.h"
#include "LightMath/Runtime/Geometry/Segment.h"

namespace Light
{
    struct LinesMesh
    {
        std::vector<Segment> lines;

    private:
        friend class LinesRendererSystem;
        std::optional<std::unique_ptr<Mesh>> linesMesh;
    };

    Light_MakeType(LinesMesh, "253F9DB5-B894-43FD-A319-A69717536850")
    {
        Light_MakeType_AddField(lines);
    }
}