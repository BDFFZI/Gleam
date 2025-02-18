struct Vertex
{
    float3 position:POSITION;
    float4 color:COLOR;
    float2 uv:TEXCOORD;
    float3 normal:NORMAL;
    float3 tangent:TANGENT;
};

[[vk::push_constant]]
cbuffer ObjectInfo
{
    float4x4 LocalToWorld;
    float4 color;
}

cbuffer WorldInfo : register(b0)
{
    float4x4 WorldToView;
    float4x4 ViewToClip;
    float Time;
}

Texture2D AlbedoTex : register(t1); //rgb：颜色，a：透明度
SamplerState AlbedoTexSampler : register(s1);
Texture2D MetallicGlossTex : register(t2); //rgb：金属度，a：光泽度
SamplerState MetallicGlossTexSampler : register(s2);
Texture2D NormalTex : register(t3); //rgb：法线
SamplerState NormalTexSampler : register(s3);

float4 TransformObjectToClip(float3 positionOS)
{
    return mul(LocalToWorld, float4(positionOS, 1));
}