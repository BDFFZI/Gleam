#pragma once
#include "GleamUI/Runtime/UI.h"
#include <ImGuizmo.h>

#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMath/Runtime/Geometry/Solid/Cuboid.h"
#include "GleamMath/Runtime/Geometry/Solid/Sphere.h"
#include "GleamMath/Runtime/LinearAlgebra/Matrix.h"
#include "GleamUI/Runtime/UISystem.h"

namespace Gleam
{
    class Handles : public System
    {
    public:
        static Handles& GetInstance();

        static float4x4& WorldToView();
        static float4x4& ViewToClip();
        static float3 DrawHandle(float3 position);
        static void DrawHandle(ImGuizmo::OPERATION operation, float4x4 localToWorld, std::optional<std::reference_wrapper<LocalTransform>> transform);

    private:
        Gleam_Engine_Friend
        
        inline static bool orthographic = false;
        inline static float4x4 worldToView = float4x4::Identity();
        inline static float4x4 viewToClip = float4x4::Identity();

        Handles(): System(UISystem, MinOrder, DefaultOrder)
        {
        }
        void Update() override;
    };
}