#include "Resources/GleamRenderingRuntime/Core.hlsl"
#include "../../../../GleamRendering/Runtime/Resources/GleamRenderingRuntime/Core.hlsl"

struct InstanceInfo
{
    float4x4 localToWorld;
    float4 albedo;
};

StructuredBuffer<InstanceInfo> instanceInfos : register(b5); //实例化渲染用

struct Fragment
{
    float4 positionCS : SV_POSITION;
    float4 color : COLOR;
};

Fragment VertexShader(Vertex vertex, uint instanceID:SV_InstanceID)
{
    Fragment output;
    output.positionCS = TransformObjectToClip(mul(instanceInfos[instanceID].localToWorld, float4(vertex.position, 1)));
    output.color = vertex.color * instanceInfos[instanceID].albedo;
    return output;
}

float4 FragmentShader(Fragment fragment): SV_Target
{
    return fragment.color;
}