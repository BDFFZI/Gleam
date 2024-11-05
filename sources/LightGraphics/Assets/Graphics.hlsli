[[vk::push_constant]]
cbuffer PushConstant
{
    float4x4 objectToWorld;
    float4x4 worldToView;
    float4x4 viewToClip;
}

struct PointVertex
{
    float3 position:POSITION;
    float4 color:COLOR;
};

struct LineVertex
{
    float3 position:POSITION;
    float4 color:COLOR;
};

struct TriangleVertex
{
    float3 position:POSITION;
    float3 normal:NORMAL;
    float4 tangent:TANGENT;
    float2 uv:TEXCOORD;
    float4 color:COLOR;
};

float4 TransformObjectToClip(float3 positionOS)
{
    float4 positionWS = mul(objectToWorld,float4(positionOS,1));
    float4 positionVS = mul(worldToView,positionWS);
    float4 positionCS = mul(viewToClip,positionVS);
    return positionCS;
}