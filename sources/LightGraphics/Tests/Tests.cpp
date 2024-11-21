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


struct Fullscreen
{
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Material> material;

    Fullscreen()
    {
        mesh = std::make_unique<Mesh>();
        mesh->SetPositions({
            float3(-1, -1, 1), float3(-1, 1, 1),
            float3(1, 1, 1), float3(1, -1, 1)
        });
        float4 red = float4(1.0f, 0.0f, 0.0f, 1.0f);
        mesh->SetColors({red, red, red, red});
        mesh->SetIndices({
            0, 1, 2,
            0, 2, 3
        });
        
        shader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout);
        material = std::make_unique<Material>(*shader);
    }
};

struct Box
{
    inline static float3 move[4] = {
        float3(-1, 1, 1),
        float3(1, 1, -1),
        float3(1, -1, 1),
        float3(-1, -1, -1),
    };

    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Texture2D> texture;
    std::unique_ptr<Material> material;

    Box()
    {
        mesh = Mesh::CreateFromRawMesh(ModelImporter::ImportObj("Assets/Cube.obj"));
        shader = Shader::CreateFromFile(
            "Assets/PositionUV.hlsl",
            std::vector{
                GLDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            });

        std::vector data(9, float4(1));
        data[4] = float4(0);
        texture = std::make_unique<Texture2D>(3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, data.data(), sizeof float4 * data.size());

        material = std::make_unique<Material>(*shader);
        material->SetTexture2D(0, *texture);
    }
};

struct WireBox
{
    struct Point
    {
        float3 position;
        float4 color;
    };

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

    std::unique_ptr<MeshT<Point>> mesh;
    std::unique_ptr<Shader> shader;
    std::unique_ptr<Material> material;

    WireBox(Box& box)
    {
        mesh = std::make_unique<MeshT<Point>>(&lineMeshLayout);
        std::vector<DefaultVertex> vertices = box.mesh->GetVertices();
        std::vector<Point> lineVertices(vertices.size());
        for (uint32_t i = 0; i < vertices.size(); i++)
        {
            lineVertices[i].position = vertices[i].position;
            lineVertices[i].color = vertices[i].color;
        }
        mesh->SetVertices(lineVertices);
        mesh->SetIndices(box.mesh->GetIndices());

        shader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, lineMeshLayout);

        material = std::make_unique<Material>(*shader);
    }
};

void Update(GLFWwindow* glfwWindow)
{
    Fullscreen fullscreen = {};
    Box box = {};
    WireBox wireBox = {box};

    Chronograph chronograph;
    chronograph.Tick();

    CommandBuffer& commandBuffer = Graphics::ApplyCommandBuffer();

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        //开始呈现
        GLCommandBuffer* glCommandBuffer = Graphics::BeginPresent();
        //开始渲染
        commandBuffer.BeginRecording();
        commandBuffer.BeginRendering(Graphics::GetPresentRenderTarget()); //设置渲染目标
        //设置背景色
        commandBuffer.ClearRenderTarget(float4(0.5, 0.5, 0.5, 1));

        commandBuffer.SetViewProjectionMatrices(
            inverse(float4x4::TRS({1, 2, -3}, {30, -20, 0}, {1, 1, 1})),
            float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f)
        );
        //测试变换
        {
            for (int i = 0; i < 4; ++i)
            {
                float4x4 objectToWorld = float4x4::TRS(
                    float3{0, 0, static_cast<float>(i) * 0.1f} + Box::move[i] * fmod(static_cast<float>(chronograph.Time()) / 6000, 1.0f),
                    {-90, 0, 0},
                    static_cast<float>(i + 1) / 4.0f
                );

                commandBuffer.Draw(*box.mesh, *box.material, objectToWorld);
            }
        }
        //测试自定义网格
        {
            commandBuffer.Draw(*wireBox.mesh, *wireBox.material, float4x4::TRS(0, 0, 2));
        }
        //测试视口和NDC
        {
            uint32_t halfWidth = Graphics::GetPresentRenderTarget().width / 2;
            uint32_t halfHeight = Graphics::GetPresentRenderTarget().height / 2;
            commandBuffer.SetViewProjectionMatrices(float4x4::Identity());
            commandBuffer.SetViewport(static_cast<int32_t>(halfWidth), static_cast<int32_t>(halfHeight), halfWidth / 2, halfHeight / 2);
            commandBuffer.Draw(*fullscreen.mesh, *fullscreen.material);
        }

        //完成渲染录制
        commandBuffer.EndRendering();
        commandBuffer.EndRecording();
        //完成呈现
        if (glCommandBuffer != nullptr)
        {
            glCommandBuffer->ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());
            Graphics::EndPresent(*glCommandBuffer);
            Graphics::WaitPresent();
        }
    }

    Graphics::ReleaseCommandBuffer(commandBuffer);
}

void main()
{
    constexpr uint32_t WIDTH = 1920 / 3;
    constexpr uint32_t HEIGHT = 1080 / 3;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Graphics", nullptr, nullptr);

    std::vector<const char*> extensions = {};
    Graphics::InitializeGLDemand(extensions);

    GL gl = GL::Initialize(window, extensions);
    Graphics::Initialize(gl);

    Update(window);

    Graphics::UnInitialize();
    GL::UnInitialize();
}
