#pragma once
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamMath/Runtime/Geometry/3D/Line.h"
#include "GleamRendering/Runtime/Asset/Mesh.h"

namespace Gleam
{
    struct LinesMesh
    {
        std::vector<Segment> lines;
        std::shared_ptr<Mesh> linesMesh = std::make_shared<Mesh>(true);

        ~LinesMesh()
        {
            SwapChain::WaitPresent();
        }
    };

    Gleam_MakeTypeWithID(LinesMesh, "253F9DB5-B894-43FD-A319-A69717536850")
    {
        Gleam_MakeType_AddField(lines);
    }
}