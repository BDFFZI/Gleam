#include "Graphics.hlsli"

struct Vertex
{
    float2 position;
    float4 color;
};

struct Fragment
{
    float4 positionCS:SV_POSITION;
    float4 color:COLOR;
    [[vk::builtin("PointSize")]] 
    float pointSize:TEXCOORD0;
};

Fragment VertexShader(Vertex vertex)
{
    Fragment fragment;
    fragment.positionCS = TransformObjectToClip(float3(vertex.position,0.5f));
    fragment.color = vertex.color;
    fragment.pointSize = 5;
    return fragment;
}

float4 FragmentShader(Fragment fragment):SV_Target
{
    return fragment.color;
}