#include <gtest/gtest.h>

#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"

// TEST(GraphicsTests, ALL)
void main()
{
    using namespace LightRuntime;

    constexpr uint32_t WIDTH = 800;
    constexpr uint32_t HEIGHT = 600;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    Graphics::Initialize(window);

    Mesh mesh = {};
    mesh.SetPositions({
        float3(0, 0.5f, 0),
        float3(0.5, -0.5, 0),
        float3(-0.5, -0.5, 0),
    });
    mesh.SetTriangles({
        0, 1, 2,
    });
    mesh.SetColors({
        color::red,
        color::green,
        color::blue,
    });

    std::string shaderCode = R"(
struct VertexInput
{
    [[vk::location(0)]]
    float3 positionOS:POSITION;
    [[vk::location(4)]]
    float4 color:COLOR;
};

struct VertexOutput
{
    float4 positionCS:SV_Position;
    float4 color:COLOR;
};

VertexOutput VertexShader(VertexInput input)
{
    VertexOutput output;
    output.positionCS = float4(input.positionOS,1);
    output.color = input.color;
    return output;
}

float4 FragmentShader(VertexOutput input):SV_Target
{
    return input.color;
}

)";
    std::vector<std::byte> vertexShader = ShaderImporter::ImportHlsl(shaderc_vertex_shader, shaderCode, "VertexShader");
    std::vector<std::byte> fragmentShader = ShaderImporter::ImportHlsl(shaderc_fragment_shader, shaderCode, "FragmentShader");
    Shader shader = {
        {
            GLShader(VK_SHADER_STAGE_VERTEX_BIT, vertexShader, "VertexShader"),
            GLShader(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader, "FragmentShader"),
        },
        {}
    };
    Material material = {shader};

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //逻辑处理
        //...

        Graphics::WaitPresentable();

        CommandBuffer* commandBuffer = Graphics::GetCommandBuffer();
        commandBuffer->BeginRecording();
        commandBuffer->BeginRendering(nullptr, true);
        commandBuffer->SetViewport({0, 0, Graphics::GetGLSwapChainExtent().x, Graphics::GetGLSwapChainExtent().y});
        commandBuffer->Draw(mesh, material);
        commandBuffer->EndRendering();
        commandBuffer->EndRecording();

        Graphics::PresentAsync(*commandBuffer);
        Graphics::WaitPresentAsync([=]
        {
            Graphics::ReleaseCommandBuffer(commandBuffer);
        });
    }

    Graphics::UnInitialize();
}
