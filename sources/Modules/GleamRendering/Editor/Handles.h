#pragma once
#include "GleamUI/Runtime/UI.h"
#include <ImGuizmo.h>

#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/LinearAlgebra/Matrix.h"

namespace Gleam
{
    class Handles
    {
    public:
        static float4x4& WorldToView();
        static float4x4& ViewToClip();
        static float3 DrawHandle(float3 position);
        static void DrawHandle(ImGuizmo::OPERATION operation, float4x4 localToWorld, std::optional<std::reference_wrapper<LocalTransform>> transform);

    private:
        inline static bool orthographic = false;
        inline static float4x4 worldToView = float4x4::Identity();
        inline static float4x4 viewToClip = float4x4::Identity();
    };
}