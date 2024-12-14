struct Vertex
{
    float3 position:POSITION;
    float3 normal:NORMAL;
    float3 tangent:TANGENT;
    float2 uv:TEXCOORD;
};

[[vk::push_constant]]
cbuffer PushConstant
{
    float4x4 MatrixMVP;
}

Texture2D albedoTex : register(t0);//rgb：颜色，a：透明度
SamplerState albedoTexSampler : register(s0);
Texture2D metallicGlossTex : register(t1);//rgb：金属度，a：光泽度
SamplerState metallicGlossTexSampler : register(s1);
Texture2D normalTex : register(t1);//rgb：法线
SamplerState normalTexSampler : register(s1);

float4 TransformObjectToClip(float3 positionOS)
{
    return mul(MatrixMVP,float4(positionOS,1));
}