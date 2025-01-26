struct Vertex
{
    float3 position:POSITION;
    float4 color:COLOR;
    float2 uv:TEXCOORD;
    float3 normal:NORMAL;
    float3 tangent:TANGENT;
};

[[vk::push_constant]]
cbuffer PushConstant
{
    float4x4 MatrixMVP;
}

Texture2D AlbedoTex : register(t0); //rgb：颜色，a：透明度
SamplerState AlbedoTexSampler : register(s0);
Texture2D MetallicGlossTex : register(t1); //rgb：金属度，a：光泽度
SamplerState MetallicGlossTexSampler : register(s1);
Texture2D NormalTex : register(t1); //rgb：法线
SamplerState NormalTexSampler : register(s1);

float4 TransformObjectToClip(float3 positionOS)
{
    return mul(MatrixMVP, float4(positionOS, 1));
}