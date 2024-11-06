﻿#include <gtest/gtest.h>

#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightImport/Runtime/ShaderImporter.h"
#include "LightMath/Runtime/Matrix.hpp"
#include "LightMath/Runtime/MatrixMath.hpp"
#include "LightUtility/Runtime/Chronograph.hpp"

using namespace Light;

struct Point
{
    float3 position;
    float4 color;
};

std::unique_ptr<Mesh> CreateMesh()
{
    return Mesh::CreateFromRawMesh(ModelImporter::ImportObj("Assets/Cube.obj"));
}
std::unique_ptr<Shader> CreateShader()
{
    StateLayout stateLayout = {};
    stateLayout.multisample.rasterizationSamples = Graphics::GetPresentSampleCount();
    return Shader::CreateFromFile(
        "Assets/PositionUV.hlsl",
        std::vector{
            GLDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        }, stateLayout);
}
std::unique_ptr<Texture2D> CreateTexture2D()
{
    std::vector data(9, float4(1));
    data[4] = float4(0);
    return std::make_unique<Texture2D>(3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, data.data(), sizeof float4 * data.size());
}
std::unique_ptr<Material> CreateMaterial(const std::unique_ptr<Shader>& shader, const std::unique_ptr<Texture2D>& texture)
{
    std::unique_ptr<Material> material = std::make_unique<Material>(*shader);
    material->SetTexture2D(0, *texture);
    return material;
}

void main()
{
    constexpr uint32_t WIDTH = 1920 / 3;
    constexpr uint32_t HEIGHT = 1080 / 3;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    GL gl = GL::Initialize(window);
    Graphics::Initialize(gl);

    auto mesh = CreateMesh();
    auto shader = CreateShader();
    auto texture = CreateTexture2D();
    auto material = CreateMaterial(shader, texture);

    std::vector<Vertex> vertices = mesh->GetVertices();
    std::unique_ptr<MeshTemplate<Point>> lineMesh = std::make_unique<MeshTemplate<Point>>();
    std::vector<Point> lineVertices(vertices.size());
    for (uint32_t i = 0; i < vertices.size(); i++)
    {
        lineVertices[i].position = vertices[i].position;
        lineVertices[i].color = vertices[i].color;
    }
    lineMesh->SetVertices(lineVertices);
    lineMesh->SetIndices(mesh->GetIndices());
    lineMesh->UpdateGLBuffer();

    GLMeshLayout lineMeshLayout = GLMeshLayout{
        sizeof(Point), std::vector{
            GLVertexAttribute{0,offsetof(Point, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{4,offsetof(Point, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        },
        VK_PRIMITIVE_TOPOLOGY_LINE_STRIP
    };

    std::unique_ptr<Shader> lineShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, {}, lineMeshLayout);
    std::unique_ptr<Material> lineMaterial = std::make_unique<Material>(*lineShader);

    float3 move[4] = {
        float3(-1, 1, 1),
        float3(1, 1, -1),
        float3(1, -1, 1),
        float3(-1, -1, -1),
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
        commandBuffer.SetViewport(0, 0, Graphics::GetPresentRenderTexture().GetWidth(), Graphics::GetPresentRenderTexture().GetHeight());
        commandBuffer.SetViewProjectionMatrices(
            inverse(float4x4::TRS({1, 2, -3}, {30, -20, 0}, {1, 1, 1})),
            float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f)
        );
        commandBuffer.BeginRendering(Graphics::GetPresentRenderTexture(), true);
        for (int i = 0; i < 4; ++i)
        {
            float4x4 objectToWorld = float4x4::TRS(
                float3{0, 0, static_cast<float>(i) * 0.1f} + move[i] * fmod(static_cast<float>(chronograph.Time()) / 6000, 1.0f),
                {-90, 0, 0},
                static_cast<float>(i + 1) / 4.0f
            );

            commandBuffer.Draw(*mesh, objectToWorld, *material);
        }
        commandBuffer.Draw(*lineMesh, float4x4::TRS(0, 0, 2), *lineMaterial);
        commandBuffer.EndRendering();

        commandBuffer.EndRecording();
        Graphics::PresentAsync(commandBuffer);

        //等待绘制完成
        Graphics::WaitPresent();
        Graphics::ReleaseCommandBuffer(commandBuffer);
    }

    material.reset();
    texture.reset();
    shader.reset();
    mesh.reset();

    lineMaterial.reset();
    lineShader.reset();
    lineMesh.reset();

    Graphics::UnInitialize();
    GL::UnInitialize();
}
