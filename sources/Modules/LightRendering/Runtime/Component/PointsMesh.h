#pragma once
#include "Renderer.h"
#include "LightGraphics/Runtime/GraphicsConfig.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightMath/Runtime/Geometry/Point.h"

namespace Light
{
    struct PointsRenderer : Renderer
    {
        inline static GLMeshLayout PointMeshLayout = {
            GraphicsConfig::DefaultVertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
        };

        std::vector<Point> points;
        std::unique_ptr<Mesh> mesh;
    };
}