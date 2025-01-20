#pragma once

namespace Light
{
    struct Camera
    {
        bool orthographic = false; //相加是正交(true)还是透视(false)
        float aspect = 16.0f / 9.0f;
        float fieldOfView = 60;
        float nearClipPlane = 0.3f;
        float farClipPlane = 1000.0f;
    };
}