#pragma once
#include "GleamUI/Runtime/UI.h"
#include <ImGuizmo.h>

#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/Geometry/Solid/Cuboid.h"
#include "GleamMath/Runtime/Geometry/Solid/Sphere.h"
#include "GleamMath/Runtime/LinearAlgebra/Matrix.h"

namespace Gleam
{
    class Gizmos
    {
    public:
        static float4x4& WorldToView();
        static float4x4& ViewToClip();

        static float3 DrawHandle(float3 position);
        static void DrawHandle(ImGuizmo::OPERATION operation, float4x4 localToWorld, std::optional<std::reference_wrapper<LocalTransform>> transform);
        static void DrawCuboid(Cuboid cuboid);
        static void DrawSphere(Sphere sphere);

    private:
        inline static float4x4 worldToView = float4x4::Identity();
        inline static float4x4 viewToClip = float4x4::Identity();
    };
}