#pragma once
#include "Renderer.h"
#include "LightMath/Runtime/Geometry/Segment.h"

namespace Light
{
    struct LinesRenderer : Renderer
    {
        std::vector<Segment> lines;
        std::unique_ptr<Mesh> mesh;
    };
}