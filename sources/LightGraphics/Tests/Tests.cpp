#include <thread>

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

//自定义顶点
struct Point
{
    float3 position;
    float4 color;
};

//自定义线框网格布局
GLMeshLayout lineMeshLayout = {
    GLVertexInput{
        sizeof(Point), {
            GLVertexAttribute{0,offsetof(Point, position), VK_FORMAT_R32G32B32_SFLOAT},
            GLVertexAttribute{4,offsetof(Point, color), VK_FORMAT_R32G32B32A32_SFLOAT},
        }
    },
    GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, false}
};

//变换测试用的移动方向
inline static float3 move[4] = {
    float3(-1, 1, 1),
    float3(1, 1, -1),
    float3(1, -1, 1),
    float3(-1, -1, -1),
};

//计时器，每帧置零
Chronograph chronograph;

std::unique_ptr<Mesh> fullScreenMesh;
std::unique_ptr<Mesh> boxMesh;
std::unique_ptr<MeshT<Point>> wireBoxMesh;

std::unique_ptr<Shader> texture2DShader;
std::unique_ptr<Shader> vertexColorShader;
std::unique_ptr<Shader> wireVertexColorShader;

std::unique_ptr<Texture2D> texture2D;

std::unique_ptr<Material> texture2DMaterial;
std::unique_ptr<Material> vertexColorMaterial;
std::unique_ptr<Material> wireVertexColorMaterial;

std::unique_ptr<RenderTexture> renderTexture;

void CreateAssets()
{
    //全屏网格
    fullScreenMesh = std::make_unique<Mesh>();
    fullScreenMesh->SetPositions({
        float3(-1, -1, 1), float3(-1, 1, 1),
        float3(1, 1, 1), float3(1, -1, 1)
    });
    fullScreenMesh->SetUVs({
        float2(0, 1), float2(0, 0),
        float2(1, 0), float2(1, 1)
    });
    float4 red = float4(1.0f, 0.0f, 0.0f, 1.0f);
    fullScreenMesh->SetColors({red, red, red, red});
    fullScreenMesh->SetIndices({
        0, 1, 2,
        0, 2, 3
    });
    //盒状网格
    boxMesh = Mesh::CreateFromRawMesh(ModelImporter::ImportObj("Assets/Cube.obj"));
    //线框盒状网格
    std::vector<DefaultVertex> vertices = boxMesh->GetVertices();
    std::vector<Point> lineVertices(vertices.size());
    for (uint32_t i = 0; i < vertices.size(); i++)
    {
        lineVertices[i].position = vertices[i].position;
        lineVertices[i].color = vertices[i].color;
    }
    wireBoxMesh = std::make_unique<MeshT<Point>>(&lineMeshLayout);
    wireBoxMesh->SetVertices(lineVertices);
    wireBoxMesh->SetIndices(boxMesh->GetIndices());

    //纹理着色器
    texture2DShader = Shader::CreateFromFile(
        "Assets/Texture2D.hlsl",
        std::vector{
            GLDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        });
    //顶点颜色着色器
    vertexColorShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout);
    //线框顶点颜色着色器
    wireVertexColorShader = Shader::CreateFromFile("Assets/VertexColor.hlsl", {}, DefaultGLStateLayout, lineMeshLayout);

    //纹理
    std::vector data(9, float4(1));
    data[4] = float4(0);
    texture2D = std::make_unique<Texture2D>(3, 3, VK_FORMAT_R32G32B32A32_SFLOAT, data.data(), sizeof float4 * data.size());

    //纹理材质球
    texture2DMaterial = std::make_unique<Material>(*texture2DShader);
    texture2DMaterial->SetTexture(0, *texture2D);
    //顶点颜色材质球
    vertexColorMaterial = std::make_unique<Material>(*vertexColorShader);
    //线框顶点颜色材质球
    wireVertexColorMaterial = std::make_unique<Material>(*wireVertexColorShader);

    //渲染纹理
    renderTexture = std::make_unique<RenderTexture>(
        Graphics::GetPresentRenderTarget().width,
        Graphics::GetPresentRenderTarget().height
    );
}

void DeleteAssets()
{
    renderTexture.reset();

    wireVertexColorMaterial.reset();
    vertexColorMaterial.reset();
    texture2DMaterial.reset();

    texture2D.reset();

    wireVertexColorShader.reset();
    vertexColorShader.reset();
    texture2DShader.reset();

    wireBoxMesh.reset();
    boxMesh.reset();
    fullScreenMesh.reset();
}

void Draw(CommandBuffer& commandBuffer, float4 background = float4(0.5, 0.5, 0.5, 1))
{
    //绘制背景色
    commandBuffer.ClearRenderTarget(background);

    //测试变换，纹理
    {
        texture2DMaterial->SetTexture(0, *texture2D);
        for (int i = 0; i < 4; ++i)
        {
            float4x4 objectToWorld = float4x4::TRS(
                float3{0, 0, static_cast<float>(i) * 0.1f} + move[i] * fmod(static_cast<float>(chronograph.Time()) / 6000, 1.0f),
                {-90, 0, 0},
                static_cast<float>(i + 1) / 4.0f
            );

            commandBuffer.Draw(*boxMesh, *texture2DMaterial, objectToWorld);
        }
    }

    //测试自定义网格
    commandBuffer.Draw(*wireBoxMesh, *wireVertexColorMaterial, float4x4::TRS(0, 0, 2));

    //测试视口和NDC
    {
        uint32_t halfWidth = commandBuffer.GetRenderTarget()->GetWidth() / 2;
        uint32_t halfHeight = commandBuffer.GetRenderTarget()->GetHeight() / 2;
        commandBuffer.SetViewProjectionMatrices(float4x4::Identity());
        commandBuffer.SetViewport(static_cast<int32_t>(halfWidth), static_cast<int32_t>(halfHeight), halfWidth / 2, halfHeight / 2);
        commandBuffer.Draw(*fullScreenMesh, *vertexColorMaterial);
    }
}

void Update(GLFWwindow* glfwWindow)
{
    //开始计时
    chronograph.Tick();

    //提前申请绘制时使用的子命令缓冲区
    std::vector<CommandBuffer*> commandBuffers = {};
    for (uint32_t i = 0; i < Graphics::GetGLSwapChainBufferCount(); ++i)
        commandBuffers.push_back(&Graphics::ApplyCommandBuffer());

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        //申请开始一次呈现，并获取本次呈现用的主命令缓冲区
        Graphics::WaitPresentable();
        GLCommandBuffer* presentCommandBuffer;
        if (Graphics::BeginPresent(&presentCommandBuffer) == false)
            continue; //当前无法呈现

        //取出绘制用的命令缓冲区开始录制绘制命令
        CommandBuffer& commandBuffer = *commandBuffers[Graphics::GetGLSwapChain()->GetCurrentBufferIndex()];
        commandBuffer.BeginRecording();

        //绘制相机1到自定义渲染目标
        commandBuffer.SetRenderTarget(*renderTexture); //设置渲染目标
        commandBuffer.SetViewProjectionMatrices( //设置视角
            inverse(float4x4::TRS({1, 2, -3}, {30, -20, 0}, {1, 1, 1})),
            float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f)
        );
        Draw(commandBuffer,0);
        commandBuffer.SetRenderTargetToNull();
        //将自定义渲染目标布局转为着色器的采样纹理
        commandBuffer.GetGLCommandBuffer().TransitionImageLayout(
            renderTexture->GetVKColorImage(),
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);


        //绘制相机2到屏幕
        commandBuffer.SetRenderTarget(Graphics::GetPresentRenderTarget()); //设置渲染目标
        commandBuffer.ClearRenderTarget();
        commandBuffer.SetViewProjectionMatrices( //设置视角
            inverse(float4x4::TRS({0, 2, 3}, {30, -180, 0}, {1, 1, 1})),
            float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f)
        );
        Draw(commandBuffer);
        //绘制相机1画面到屏幕
        commandBuffer.SetViewport(50, 50, 160, 90);
        commandBuffer.SetViewProjectionMatricesToIdentity();
        texture2DMaterial->SetTexture(0, *renderTexture);
        commandBuffer.Draw(*fullScreenMesh, *texture2DMaterial);


        //结束绘制命令录制
        commandBuffer.EndRecording();

        //将绘制命令提交到主命令缓冲区
        presentCommandBuffer->ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());
        //将主命令缓冲区提交到管道，完成呈现
        Graphics::EndPresent(*presentCommandBuffer);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    Graphics::WaitPresent();

    for (uint32_t i = 0; i < Graphics::GetGLSwapChainBufferCount(); ++i)
        Graphics::ReleaseCommandBuffer(*commandBuffers[i]);
}

int main()
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

    CreateAssets();

    Update(window);

    DeleteAssets();

    Graphics::UnInitialize();
    GL::UnInitialize();

    return 0;
}
