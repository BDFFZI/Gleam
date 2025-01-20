#pragma once
#include "Renderer.hpp"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightMath/Runtime/Geometry/Segment.h"

namespace Light
{
    struct LinesRenderer : Renderer
    {
        inline static GLMeshLayout LineMeshLayout = {
            GraphicsPreset::DefaultVertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
        };

        std::vector<Segment> lines;
        std::unique_ptr<Mesh> mesh;
    };
}