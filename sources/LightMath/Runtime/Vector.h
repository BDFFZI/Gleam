#pragma once

namespace LightEngine
{
    struct float2
    {
        float x;
        float y;

        float2() = default;
        float2(float value);
        float2(float x, float y);
    };

    struct float3
    {
        float x;
        float y;
        float z;

        float3() = default;
        float3(float value);
        float3(float x, float y, float z);
        float3(float2 xy, float z);
    };

    struct float4
    {
        float x;
        float y;
        float z;
        float w;

        float4() = default;
        float4(float value);
        float4(float x, float y, float z, float w);
        float4(float3 xyz, float w);
    };
}
