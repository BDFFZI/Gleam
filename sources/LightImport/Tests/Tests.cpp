#include "LightImport/Runtime/ShaderImporter.h"

using namespace Light;

void main()
{
    ShaderImporter::ImportHlsl(
        shaderc_vertex_shader,
        R"(
cbuffer ubo : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
}

Texture2D tex : register(t1);
SamplerState texSampler : register(s1);

struct VSInput
{
    float3 positionOS : POSITION0;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 positionCS : SV_Position;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};

VSOutput VertexShader(VSInput input, uint VertexIndex: SV_VertexID)
{
    VSOutput output = (VSOutput)0;
    output.color = input.color;
    output.uv = input.uv;
    output.positionCS = mul(proj, mul(view, mul(model, float4(input.positionOS, 1.0))));
    return output;
}

float4 FragmentShader(VSOutput input): SV_Target
{
    return tex.Sample(texSampler, input.uv) * float4(input.color,1);
}
)",
        "VertexShader"
    );
}
