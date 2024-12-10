#include "../../Assets/Graphics.hlsli"

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

struct Fragment
{
    float4 positionCS:SV_Position;
    float2 uv:TEXCOORD;
    float4 color:COLOR;
};

Fragment VertexShader(DefaultVertex vertex)
{
    Fragment output;
    output.positionCS = TransformObjectToClip(vertex.position);
    output.uv = vertex.uv;
    output.color = float4(vertex.position,1);
    return output;
}

float4 FragmentShader(Fragment fragment):SV_Target
{
    return fragment.color * tex.Sample(texSampler,fragment.uv);
}