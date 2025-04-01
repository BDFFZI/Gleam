#pragma once
#include "Renderer.h"
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"

namespace Gleam
{
    struct PointsMesh
    {
        std::vector<Point> points;
        std::shared_ptr<Mesh> pointsMesh = std::make_shared<Mesh>(true);

        ~PointsMesh()
        {
            SwapChain::WaitPresent();
        }
    };

    Gleam_MakeTypeWithID(PointsMesh, "B063D6E1-C2DE-4E37-AE7F-0F47502C8A0B")
    {
        Gleam_MakeType_AddField(points);
    }
}