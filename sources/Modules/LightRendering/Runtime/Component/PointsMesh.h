#pragma once
#include "Renderer.h"
#include "LightMath/Runtime/Geometry/Point.h"

namespace Light
{
    struct PointsMesh
    {
        std::vector<Point> points;

    private:
        friend class PointsRendererSystem;
        std::unique_ptr<Mesh> pointsMesh;
    };
}