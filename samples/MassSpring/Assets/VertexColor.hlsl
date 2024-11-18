#include "Graphics.hlsli"

struct Fragment
{
    float4 positionCS:SV_POSITION;
    float4 color:COLOR;
};

Fragment VertexShader(DefaultVertex vertex)
{
    Fragment fragment;
    fragment.positionCS = TransformObjectToClip(vertex.position);
    fragment.color = vertex.color;
    return fragment;
}

float4 FragmentShader(Fragment fragment,[[vk::builtin("PointSize")]] out float PointSize:PSIZE):SV_Target
{
    return fragment.color;
}