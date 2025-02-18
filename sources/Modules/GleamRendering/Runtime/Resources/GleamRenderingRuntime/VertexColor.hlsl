#include "Core.hlsl"

struct Fragment
{
    float4 positionCS : SV_POSITION;
    float4 vertexColor : COLOR;
    [[vk::builtin("PointSize")]]
    float pointSize:TEXCOORD0;
};

Fragment VertexShader(Vertex vertex)
{
    Fragment output;
    output.positionCS = TransformObjectToClip(vertex.position);
    output.vertexColor = vertex.color;
    output.pointSize = 5;
    return output;
}

float4 FragmentShader(Fragment fragment): SV_Target
{
    return fragment.vertexColor * Albedo;
}