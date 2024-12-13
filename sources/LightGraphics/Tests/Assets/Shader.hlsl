#include "../Assets/Graphics.hlsl"

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
    return albedoTex.Sample(albedoTexSampler,fragment.uv);
}