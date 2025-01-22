#pragma once
#include "Renderer.h"
#include "LightMath/Runtime/Geometry/Point.h"

namespace Light
{
    struct PointsRenderer : Renderer
    {
        std::vector<Point> points;
        std::unique_ptr<Mesh> mesh;
    };
}