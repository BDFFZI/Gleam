struct Vertex
{
    float3 position:POSITION;
    float4 color:COLOR;
    float2 uv:TEXCOORD;
    float3 normal:NORMAL;
    float3 tangent:TANGENT;
};

cbuffer WorldInfo : register(b0)
{
    float Time;
}

[[vk::push_constant]]
cbuffer ObjectInfo
{
    float4x4 LocalToClip;
}

cbuffer MaterialInfo : register(b1)
{
    float4 Albedo;
    float Metallic;
    float Smoothness;
}
Texture2D AlbedoTex : register(t2); //rgb：颜色，a：透明度
SamplerState AlbedoTexSampler : register(s2);
Texture2D MetallicGlossTex : register(t3); //rgb：金属度，a：光泽度
SamplerState MetallicGlossTexSampler : register(s3);
Texture2D NormalTex : register(t4); //rgb：法线
SamplerState NormalTexSampler : register(s4);

float4 TransformObjectToClip(float3 positionOS)
{
    return mul(LocalToClip, float4(positionOS, 1));
}