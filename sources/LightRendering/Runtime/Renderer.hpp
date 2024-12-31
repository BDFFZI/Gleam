#pragma once
#include "LightMath/Runtime/Geometry/Bounds.hpp"
#include "LightMath/Runtime/Geometry/Line.hpp"
#include "LightPresentation/Runtime/Graphics/GraphicsAssets.h"
#include "LightPresentation/Runtime/Graphics/Mesh.h"

namespace Light
{
    struct Renderer
    {
        MaterialAsset* material;
        Bounds bounds;
    };

    struct LinesRenderer : Renderer
    {
        inline static GLMeshLayout LineMeshLayout = {
            GraphicsPreset::DefaultVertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_LIST, false}
        };

        std::vector<Line> lines;
        std::unique_ptr<Mesh> mesh;
    };

    struct PointsRenderer : Renderer
    {
        inline static GLMeshLayout PointMeshLayout = {
            GraphicsPreset::DefaultVertexInput,
            GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_POINT_LIST, false}
        };

        std::vector<Point> points;
        std::unique_ptr<Mesh> mesh;
    };
}