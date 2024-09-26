#include <gtest/gtest.h>

#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"

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
        float3(0, 0, 0), float3(0, 1, 0),
        float3(1, 1, 0), float3(1, 0, 0),
    });
    mesh.SetTriangles({
        0, 1, 2,
        0, 2, 3
    });
    Shader shader = {
        {
            GLShader("C:/Users/MediVision/Desktop/Light/sources/LightGL/Tests/assets/vert.spv", VK_SHADER_STAGE_VERTEX_BIT, "main"),
            GLShader("C:/Users/MediVision/Desktop/Light/sources/LightGL/Tests/assets/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, "main"),
        },
        {}
    };
    Material material = {shader};

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        //逻辑处理

        Graphics::WaitPresent();
        Graphics::Present([&](CommandBuffer& commandBuffer)
        {
            commandBuffer.BeginRecording();
            commandBuffer.SetRenderTarget(nullptr);
            commandBuffer.SetViewport({0, 0, WIDTH, HEIGHT});
            commandBuffer.Draw(mesh, material);
            commandBuffer.EndRecording();
        });
    }

    Graphics::UnInitialize();
}
