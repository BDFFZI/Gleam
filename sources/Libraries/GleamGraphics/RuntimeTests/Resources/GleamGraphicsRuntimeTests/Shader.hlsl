[[vk::push_constant]]
cbuffer PushConstant
{
    float4x4 MatrixMVP;
}

Texture2D albedoTex : register(t0);
SamplerState albedoTexSampler : register(s0);
StructuredBuffer<float4x4> matrixMVPs : register(b1); //实例化渲染用

struct Vertex
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
    uint instanceID : SV_InstanceID;
};

struct Fragment
{
    float4 positionCS : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

Fragment VertexShader(Vertex vertex)
{
    Fragment output;
    if (vertex.instanceID == 0)
        output.positionCS = mul(MatrixMVP, float4(vertex.position, 1));
    else
        output.positionCS = mul(matrixMVPs[vertex.instanceID], float4(vertex.position, 1));
    output.uv = vertex.uv;
    output.color = vertex.color;
    return output;
}

float4 FragmentShader(Fragment fragment): SV_Target
{
    return albedoTex.Sample(albedoTexSampler, fragment.uv) * fragment.color;
}