#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"
#include "LightMath/Runtime/Matrix.hpp"
#include "LightUtility/Runtime/Chronograph.hpp"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightGraphics/Runtime/Shader.h"
#include "LightGraphics/Runtime/Texture2D.h"
#include "LightGraphics/Runtime/Material.h"
#include "LightMath/Runtime/MatrixMath.hpp"

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
    return Shader::CreateFromFile(
        "Assets/PositionUV.hlsl",
        std::vector{
            GLDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        });
}
std::unique_ptr<Texture2D> CreateTexture2D()
{
    std::vector data(9, float4(1));
    data[4] = float4(0);
    return std::make_unique<Texture2D>(3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, data.data(), sizeof float4 * data.size());
}
std::unique_ptr<Material> CreateMaterial(Shader* shader, const std::unique_ptr<Texture2D>& texture)
{
    std::unique_ptr<Material> material = std::make_unique<Material>(*shader);
    material->SetTexture2D(0, *texture);
    return material;
}

struct Rect
{
    Rect()
    {
        mesh = std::make_unique<Mesh>();
        mesh->SetPositions({
            float3(-1, -1, 0.5f), float3(-1, 1, 0.5f),
            float3(1, 1, 0.5f), float3(1, -1, 0.5f)
        });
        float4 red = float4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->SetColors({red, red, red, red});
        mesh->SetTangents({
            0, 1, 2,
            0, 2, 3
        });

        shader = Shader::CreateFromFile("Assets/VertexColor.hlsl");
        material = std::make_unique<Material>(*shader);
    }

    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Material> material;
};

void main()
{
    constexpr uint32_t WIDTH = 1920 / 3;
    constexpr uint32_t HEIGHT = 1080 / 3;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

    std::vector<const char*> extensions = {};
    Graphics::InitializeGLDemand(extensions);
    GL gl = GL::Initialize(window, extensions);
    Graphics::Initialize(gl);

    auto mesh = CreateMesh();
    auto shader = CreateShader();
    auto texture = CreateTexture2D();
    auto material = CreateMaterial(shader.get(), texture);

    //创建线框网格
    GLMeshLayout lineMeshLayout = {
        GLVertexInput{
            sizeof(Point), {
                GLVertexAttribute{0,offsetof(Point, position), VK_FORMAT_R32G32B32_SFLOAT},
                GLVertexAttribute{4,offsetof(Point, color), VK_FORMAT_R32G32B32A32_SFLOAT},
            }
        },
        GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, false}
    };
    std::unique_ptr<MeshT<Point>> lineMesh = std::make_unique<MeshT<Point>>(&lineMeshLayout);
    std::vector<DefaultVertex> vertices = mesh->GetVertices();
    std::vector<Point> lineVertices(vertices.size());
    for (uint32_t i = 0; i < vertices.size(); i++)
    {
        lineVertices[i].position = vertices[i].position;
        lineVertices[i].color = vertices[i].color;
    }
    lineMesh->SetVertices(std::move(lineVertices));
    lineMesh->SetIndices(mesh->GetIndices());

    std::unique_ptr<Shader> lineShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, lineMeshLayout);
    std::unique_ptr<Material> lineMaterial = std::make_unique<Material>(*lineShader);

    float3 move[4] = {
        float3(-1, 1, 1),
        float3(1, 1, -1),
        float3(1, -1, 1),
        float3(-1, -1, -1),
    };

    Chronograph chronograph;
    chronograph.Tick();

    CommandBuffer& commandBuffer = Graphics::ApplyCommandBuffer();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        GLCommandBuffer* glCommandBuffer = Graphics::BeginPresent();
        //开始绘制
        commandBuffer.BeginRecording();
        commandBuffer.BeginRendering(Graphics::GetPresentRenderTexture(), true);
        {
            // commandBuffer.SetViewport(0, 0, Graphics::GetPresentRenderTexture().GetWidth(), Graphics::GetPresentRenderTexture().GetHeight());
            commandBuffer.SetViewProjectionMatrices(
                inverse(float4x4::TRS({1, 2, -3}, {30, -20, 0}, {1, 1, 1})),
                float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f)
            );
            for (int i = 0; i < 4; ++i)
            {
                float4x4 objectToWorld = float4x4::TRS(
                    float3{0, 0, static_cast<float>(i) * 0.1f} + move[i] * fmod(static_cast<float>(chronograph.Time()) / 6000, 1.0f),
                    {-90, 0, 0},
                    static_cast<float>(i + 1) / 4.0f
                );

                commandBuffer.Draw(*mesh, *material, objectToWorld);
            }
            commandBuffer.Draw(*lineMesh, *lineMaterial, float4x4::TRS(0, 0, 2));
        }
        commandBuffer.EndRendering();
        commandBuffer.EndRecording();

        //等待绘制完成
        if (glCommandBuffer != nullptr)
        {
            glCommandBuffer->ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());
            Graphics::EndPresent(*glCommandBuffer);
            Graphics::WaitPresent();
        }
    }

    Graphics::ReleaseCommandBuffer(commandBuffer);

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
