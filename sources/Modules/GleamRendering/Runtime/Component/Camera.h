#pragma once
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamGraphics/Runtime/Resource/GRenderTarget/GRenderTarget.h"

namespace Gleam
{
    struct Camera
    {
        //投影参数
        bool orthographic = false; //相加是正交(true)还是透视(false)
        float nearClipPlane = 0.1f;
        float farClipPlane = 1000.0f;
        float halfHeight = 5; //正交相机视野范围
        float fieldOfView = 60; //透视相机视野范围
        //渲染参数
        std::optional<std::reference_wrapper<GRenderTarget>> renderTarget = std::nullopt;
        float depth = 0;
        float4 background = 0;
    };

    Gleam_MakeType(Camera, "AE2D3669-EC8B-4E5C-99AE-460E7253BD59")
    {
        Gleam_MakeType_AddField(orthographic);
        Gleam_MakeType_AddField(nearClipPlane);
        Gleam_MakeType_AddField(farClipPlane);
        Gleam_MakeType_AddField(halfHeight);
        Gleam_MakeType_AddField(fieldOfView);
        Gleam_MakeType_AddField(depth);
        Gleam_MakeType_AddField(background);
    }

    struct ViewToClip
    {
        float4x4 value;
    };

    Gleam_MakeType(ViewToClip, "")
    {
        Gleam_MakeType_AddField(value);
    }

    struct WorldToClip
    {
        float4x4 value;
    };

    Gleam_MakeType(WorldToClip, "")
    {
        Gleam_MakeType_AddField(value);
    }

    struct ScreenToClip
    {
        float4x4 value;
    };

    Gleam_MakeType(ScreenToClip, "")
    {
        Gleam_MakeType_AddField(value);
    }

    struct ScreenToWorld
    {
        float4x4 value;
    };

    Gleam_MakeType(ScreenToWorld, "")
    {
        Gleam_MakeType_AddField(value);
    }
}