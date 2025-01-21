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
    // output.positionCS = TransformObjectToClip(vertex.position);
    output.positionCS = 1;
    output.uv = vertex.uv;
    output.color = vertex.color;
    return output;
}

float4 FragmentShader(Fragment fragment):SV_Target
{
    // return albedoTex.Sample(albedoTexSampler,fragment.uv) * fragment.color;
    return 1;
}