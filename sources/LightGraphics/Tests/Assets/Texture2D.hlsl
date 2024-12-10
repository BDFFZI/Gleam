#include "../../Assets/Graphics.hlsli"

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

struct Vertex
{
    [[vk::location(0)]]
    float3 position:POSITION;
    [[vk::location(3)]]
    float2 uv:TEXCOORD;
};

struct Fragment
{
    float4 positionCS:SV_POSITION;
    float2 uv:TEXCOORD;
};

Fragment VertexShader(Vertex vertex)
{
    Fragment output;
    output.positionCS = TransformObjectToClip(vertex.position);
    output.uv = vertex.uv;
    return output;
}

float4 FragmentShader(Fragment fragment):SV_Target
{
    return tex.Sample(texSampler,fragment.uv);
}