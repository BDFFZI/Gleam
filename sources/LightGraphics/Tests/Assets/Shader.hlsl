[[vk::push_constant]]
cbuffer PushConstant
{
    float4x4 MatrixMVP;
}

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

struct Vertex
{
    float3 position:POSITION;
    float2 uv:TEXCOORD;
    float4 color:COLOR;
};

struct Fragment
{
    float4 positionCS:SV_POSITION;
    float2 uv:TEXCOORD;
    float4 color:COLOR;
};

Fragment VertexShader(Vertex vertex)
{
    Fragment output;
    output.positionCS = mul(MatrixMVP,float4(vertex.position,1));
    output.uv = vertex.uv;
    output.color = vertex.color;
    return output;
}

float4 FragmentShader(Fragment fragment):SV_Target
{
    return tex.Sample(texSampler,fragment.uv) * fragment.color;
}