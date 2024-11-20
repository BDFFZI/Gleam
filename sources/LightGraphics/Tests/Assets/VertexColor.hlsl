#include "../../Assets/Graphics.hlsli"

struct Vertex
{
    [[vk::location(0)]]
    float3 position:POSITION;
    [[vk::location(4)]]
    float4 color:COLOR;
};

struct Fragment
{
    float4 positionCS:SV_POSITION;
    float4 color:COLOR;
};

Fragment VertexShader(Vertex vertex)
{
    Fragment fragment;
    fragment.positionCS = TransformObjectToClip(vertex.position);
    fragment.color = vertex.color;
    return fragment;
}

float4 FragmentShader(Fragment fragment):SV_Target
{
    return fragment.color;
}