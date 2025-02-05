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
        std::optional<std::unique_ptr<Mesh>> pointsMesh;
    };

    Light_MakeType(PointsMesh, "B063D6E1-C2DE-4E37-AE7F-0F47502C8A0B")
    {
        Light_MakeType_AddField(points);
    }
}