#pragma once
#include "Renderer.h"
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"

namespace Gleam
{
    struct PointsMesh
    {
        std::vector<Point> points;

        PointsMesh() = default;
        PointsMesh(PointsMesh&&) = default;
        PointsMesh& operator=(PointsMesh&&) = default;
        ~PointsMesh()
        {
            SwapChain::WaitPresent();
        }

    private:
        friend class PointsRendererSystem;
        std::optional<std::unique_ptr<Mesh>> pointsMesh;
    };

    Gleam_MakeType(PointsMesh, "B063D6E1-C2DE-4E37-AE7F-0F47502C8A0B")
    {
        Gleam_MakeType_AddField(points);
    }
}