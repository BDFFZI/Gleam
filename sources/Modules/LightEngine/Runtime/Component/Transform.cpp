#include "Transform.h"

#include "LightMath/Runtime/MatrixMath.h"

namespace Light
{
    float3 Transform::GetLocalPosition() const
    {
        return localPosition;
    }
    float3 Transform::GetLocalEulerAngles() const
    {
        return localEulerAngles;
    }
    float3 Transform::GetLocalScale() const
    {
        return localScale;
    }
    // float3 Transform::GetPosition() const
    // {
    //     return position;
    // }
    // float3 Transform::GetEulerAngles() const
    // {
    //     return eulerAngles;
    // }
    // float3 Transform::GetScale() const
    // {
    //     return scale;
    // }
    float3 Transform::GetRight() const
    {
        float3 forward = mul(static_cast<float3x3>(GetLocalToWorldMatrix()), float3(1, 0, 0));
        return normalize(forward);
    }
    float3 Transform::GetUp() const
    {
        float3 forward = mul(static_cast<float3x3>(GetLocalToWorldMatrix()), float3(0, 1, 0));
        return normalize(forward);
    }
    float3 Transform::GetForward() const
    {
        float3 forward = mul(static_cast<float3x3>(GetLocalToWorldMatrix()), float3(0, 0, 1));
        return normalize(forward);
    }
    float4x4 Transform::GetLocalToWorldMatrix() const
    {
        float4x4 localToWorld = float4x4::TRS(
            GetLocalPosition(),
            GetLocalEulerAngles(),
            GetLocalScale());
        return localToWorld;
    }
    float4x4 Transform::GetWorldToLocalMatrix() const
    {
        return inverse(GetLocalToWorldMatrix());
    }

    void Transform::SetLocalPosition(const float3 newPosition)
    {
        localPosition = newPosition;
    }
    void Transform::SetLocalEulerAngles(const float3 newEulerAngles)
    {
        localEulerAngles = newEulerAngles;
    }
    void Transform::SetLocalScale(const float3 newScale)
    {
        localScale = newScale;
    }
}