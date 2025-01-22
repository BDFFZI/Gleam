#pragma once
#include "LightMath/Runtime/Vector.h"
#include "LightGraphics/Runtime/Graphics.h"

namespace Light
{
    struct Camera
    {
        //投影参数
        bool orthographic = false; //相加是正交(true)还是透视(false)
        float halfHeight = 5;
        float aspect = 16.0f / 9.0f;
        float fieldOfView = 60;
        float nearClipPlane = 0.3f;
        float farClipPlane = 1000.0f;
        //渲染参数
        float depth = 0;
        GRenderTarget* renderTarget = nullptr;
        float4 background = 0;
    };
}