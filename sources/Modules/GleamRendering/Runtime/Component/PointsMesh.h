#pragma once
#include "Renderer.h"
#include "GleamMath/Runtime/Geometry/Point.h"

namespace Gleam
{
    struct PointsMesh
    {
        std::vector<Point> points;

    private:
        friend class PointsRendererSystem;
        std::optional<std::unique_ptr<Mesh>> pointsMesh;
    };

    Gleam_MakeType(PointsMesh, "B063D6E1-C2DE-4E37-AE7F-0F47502C8A0B")
    {
        Gleam_MakeType_AddField(points);
    }
}