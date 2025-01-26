#pragma once
#include "LightMath/Runtime/Matrix.h"
#include "LightMath/Runtime/Vector.h"
#include "LightReflection/Runtime/Type.h"

namespace Light
{
    class Transform
    {
    public:
        float3 GetLocalPosition() const;
        float3 GetLocalEulerAngles() const;
        float3 GetLocalScale() const;
        // float3 GetPosition() const;
        // float3 GetEulerAngles() const;
        // float3 GetScale() const;
        float3 GetRight() const;
        float3 GetUp() const;
        float3 GetForward() const;
        float4x4 GetLocalToWorldMatrix() const;
        float4x4 GetWorldToLocalMatrix() const;

        void SetLocalPosition(float3 newPosition);
        void SetLocalEulerAngles(float3 newEulerAngles);
        void SetLocalScale(float3 newScale);

    private:
        Light_MakeType_Friend

        float3 localPosition = 0;
        float3 localEulerAngles = 0;
        float3 localScale = 1;
        // float3 position;
        // float3 eulerAngles;
        // float3 scale;
        // float4x4 localToWorldMatrix;
    };

    Light_MakeType(Transform, "10122F01-1004-4250-B5B0-B5E0F6B271F1")
    {
        Light_MakeType_AddField(localPosition);
        Light_MakeType_AddField(localEulerAngles);
        Light_MakeType_AddField(localScale);
    }
}