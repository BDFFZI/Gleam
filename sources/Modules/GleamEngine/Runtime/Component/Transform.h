#pragma once
#include "GleamMath/Runtime/Quaternion.h"
#include "GleamMath/Runtime/LinearAlgebra/Matrix.h"
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    struct LocalTransform
    {
        float3 position;
        Quaternion rotation = {0, 0, 0, 1};
        float3 scale = 1;
    };

    struct LocalToWorld
    {
        float3 GetRight() const { return value._m00_m10_m20; }
        float3 GetUp() const { return value._m01_m11_m21; }
        float3 GetForward() const { return value._m02_m12_m22; }

        float4x4 value;
    };

    Gleam_MakeType(LocalTransform, "10122F01-1004-4250-B5B0-B5E0F6B271F1")
    {
        Gleam_MakeType_AddField(position);
        Gleam_MakeType_AddField(rotation);
        Gleam_MakeType_AddField(scale);
    }

    Gleam_MakeType(LocalToWorld, "07659F3B-B85C-4BE0-A2D0-289986D99034")
    {
        Gleam_MakeType_AddField(value);
    }
}