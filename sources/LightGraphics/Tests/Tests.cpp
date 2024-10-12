#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"
#include "LightUtility/Runtime/Chronograph.h"

using namespace LightRuntime;

struct PushConstantBuffer
{
    alignas(16) float time;
    alignas(16) Vector3 scale;
    alignas(16) Vector3 move;
};

std::unique_ptr<Mesh> CreateMesh()
{
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

    mesh->SetPositions({
        Vector3(0, 0.5f, 0),
        Vector3(0.5, -0.5, 0),
        Vector3(-0.5, -0.5, 0),
    });
    mesh->SetUVs({
        Vector2(0.5f, 1.0f),
        Vector2(1, 0),
        Vector2(0, 0),
    });
    mesh->SetColors({
        Color::red,
        Color::green,
        Color::blue,
    });
    mesh->SetTriangles({
        0, 1, 2,
    });
    mesh->UpdateGLBuffer();

    return mesh;
}
std::unique_ptr<Shader> CreateShader()
{
    std::string shaderCode = R"(
[[vk::push_constant]]
cbuffer PushConstantBuffer
{
    float time;
    float3 scale;
    float3 move;
}

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

struct VertexInput
{
    [[vk::location(0)]]
    float3 positionOS:POSITION;
    [[vk::location(3)]]
    float2 uv:TEXCOORD;
    [[vk::location(4)]]
    float4 color:COLOR;
};

struct VertexOutput
{
    float4 positionCS:SV_Position;
    float2 uv:TEXCOORD;
    float4 color:COLOR;
};

VertexOutput VertexShader(VertexInput input)
{
    float3 offset = move;
    offset.xy *= time * 2 % 5;

    VertexOutput output;
    output.positionCS = float4(input.positionOS * scale + offset,1);
    output.uv = input.uv;
    output.color = input.color;
    return output;
}

float4 FragmentShader(VertexOutput input):SV_Target
{
    return input.color * tex.Sample(texSampler,input.uv);
}

)";
    std::vector<std::byte> vertexShader = ShaderImporter::ImportHlsl(shaderc_vertex_shader, shaderCode, "VertexShader");
    std::vector<std::byte> fragmentShader = ShaderImporter::ImportHlsl(shaderc_fragment_shader, shaderCode, "FragmentShader");

    std::unique_ptr<Shader> shader = std::make_unique<Shader>(
        std::vector{
            GLShader(VK_SHADER_STAGE_VERTEX_BIT, vertexShader, "VertexShader"),
            GLShader(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader, "FragmentShader"),
        },
        std::vector{
            GLDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        },
        std::vector{
            VkPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof PushConstantBuffer),
        });
    return shader;
}
std::unique_ptr<Texture2D> CreateTexture2D()
{
    std::vector data(9, Color::white);
    data[4] = Color::black;
    return std::make_unique<Texture2D>(3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, data.data(), sizeof Color * data.size());
}
std::unique_ptr<Material> CreateMaterial(const std::unique_ptr<Shader>& shader, const std::unique_ptr<Texture2D>& texture)
{
    std::unique_ptr<Material> material = std::make_unique<Material>(*shader);
    material->SetTexture2D(0, *texture);
    return material;
}

// TEST(GraphicsTests, ALL)
void main()
{
    constexpr uint32_t WIDTH = 800;
    constexpr uint32_t HEIGHT = 600;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    Graphics::Initialize(window);

    auto mesh = CreateMesh();
    auto shader = CreateShader();
    auto texture = CreateTexture2D();
    auto material = CreateMaterial(shader, texture);

    Vector3 move[4] = {
        Vector3(-0.1f, 0.1f, 0.99f),
        Vector3(0.1f, 0.1f, 0.75f),
        Vector3(-0.1f, -0.1f, 0.25f),
        Vector3(0.1f, -0.1f, 0.0f),
    };

    Chronograph chronograph;
    chronograph.Tick();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //模拟逻辑处理
        //...

        //逻辑处理完成，开始绘制
        CommandBuffer& commandBuffer = Graphics::GetCommandBuffer();
        commandBuffer.BeginRecording();
        commandBuffer.SetViewport(0, 0, Graphics::GetGLSwapChainExtent().x, Graphics::GetGLSwapChainExtent().y);
        commandBuffer.BeginRendering(nullptr, true);
        for (int i = 0; i < 4; ++i)
        {
            PushConstantBuffer pushConstantBuffer = {
                static_cast<float>(chronograph.Time()) / 1000,
                static_cast<float>(4 - i) / 4.0f, move[i]
            };
            commandBuffer.PushConstant(material->GetShader(), 0, &pushConstantBuffer);
            commandBuffer.Draw(*mesh, *material);
        }
        commandBuffer.EndRendering();
        commandBuffer.EndRecording();
        Graphics::PresentAsync(commandBuffer);

        //等待绘制完成
        Graphics::WaitPresent();
        Graphics::ReleaseCommandBuffer(commandBuffer);
    }

    Graphics::UnInitialize();
}
