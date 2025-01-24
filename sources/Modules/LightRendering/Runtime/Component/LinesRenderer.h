#pragma once
#include "Renderer.h"
#include "LightMath/Runtime/Geometry/Segment.h"

namespace Light
{
    struct LinesRenderer : Renderer
    {
        std::vector<Segment> lines;

    private:
        friend class LinesRendererSystem;
        std::unique_ptr<Mesh> linesMesh;
    };
}