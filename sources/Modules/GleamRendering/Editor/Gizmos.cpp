#include "Gizmos.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"

namespace Gleam
{
    float4x4& Gizmos::WorldToView()
    {
        return worldToView;
    }
    float4x4& Gizmos::ViewToClip()
    {
        return viewToClip;
    }
    float3 Gizmos::DrawHandle(const float3 position)
    {
        float4x4 localToWorld = float4x4::Translate(position);
        Manipulate(
            reinterpret_cast<float*>(&worldToView),
            reinterpret_cast<float*>(&viewToClip),
            ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL,
            reinterpret_cast<float*>(&localToWorld)
        );
        return DecomposePosition(localToWorld);
    }
    void Gizmos::DrawHandle(ImGuizmo::OPERATION operation, float4x4 localToWorld, std::optional<std::reference_wrapper<LocalTransform>> transform)
    {
        //绘制
        Manipulate(
            reinterpret_cast<float*>(&worldToView),
            reinterpret_cast<float*>(&viewToClip),
            operation, ImGuizmo::LOCAL,
            reinterpret_cast<float*>(&localToWorld)
        );
        //解析矩阵
        if (transform.has_value())
        {
            LocalTransform& transformValue = transform.value().get();
            float3 position;
            float3 rotation;
            float3 scale;
            DecomposeTRS(
                localToWorld,
                position, rotation, scale
            );
            transformValue.position = position;
            transformValue.rotation = Quaternion::Euler(rotation);
            transformValue.scale = scale;
        }
    }
    void Gizmos::DrawCuboid(Cuboid cuboid)
    {
    }
    void Gizmos::DrawSphere(Sphere sphere)
    {
    }
}