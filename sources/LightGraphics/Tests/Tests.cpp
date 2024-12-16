#include <thread>

#include "LightGraphics/Runtime/CommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"
#include "LightMath/Runtime/Matrix.hpp"
#include "LightUtility/Runtime/Chronograph.hpp"
#include "LightGraphics/Runtime/Mesh.h"
#include "LightGraphics/Runtime/Shader.h"
#include "LightGraphics/Runtime/Texture.h"
#include "LightGraphics/Runtime/Material.h"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightMath/Runtime/MatrixMath.hpp"

using namespace Light;

//自定义顶点
struct Point
{
    float3 position;
    float2 uv;
    float4 color;
};

GLVertexInput VertexInput = GLVertexInput{
    sizeof(Point), {
        GLVertexAttribute{0,offsetof(Point, position), VK_FORMAT_R32G32B32_SFLOAT},
        GLVertexAttribute{1,offsetof(Point, uv), VK_FORMAT_R32G32_SFLOAT},
        GLVertexAttribute{2,offsetof(Point, color), VK_FORMAT_R32G32B32A32_SFLOAT},
    }
};

//线框条带网格布局
GLMeshLayout LineMeshLayout = {
    VertexInput,
    GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, false}
};

//变换测试用的移动方向
inline float3 Move[4] = {
    float3(-1, 1, 1),
    float3(1, 1, -1),
    float3(1, -1, 1),
    float3(-1, -1, -1),
};

//计时器，每帧置零
Chronograph chronograph;

std::unique_ptr<Mesh> fullScreenMesh;
std::unique_ptr<Mesh> boxMesh;
std::unique_ptr<MeshT<Point>> customBoxMesh;

std::unique_ptr<Shader> shader;
std::unique_ptr<Shader> lineShader;

std::unique_ptr<Texture2D> imageTexture2D;
std::unique_ptr<Texture2D> whiteTexture2D;

std::unique_ptr<Material> material;
std::unique_ptr<Material> lineMaterial;

std::unique_ptr<RenderTexture> renderTexture;

void CreateAssets()
{
    //全屏网格
    fullScreenMesh = std::make_unique<Mesh>();
    fullScreenMesh->SetPositions({
        float3(-1, -1, 1),
        float3(-1, 1, 1),
        float3(1, 1, 1),
        float3(1, -1, 1),
    });
    fullScreenMesh->SetUVs({
        float2(0, 1),
        float2(0, 0),
        float2(1, 0),
        float2(1, 1),
    });
    fullScreenMesh->SetIndices({
        0, 1, 2,
        0, 2, 3
    });
    //盒状网格
    boxMesh = std::make_unique<Mesh>(ModelImporter::ImportObj("Assets/Cube.obj"));
    //自定义盒状网格
    const std::vector<GraphicsPreset::Vertex>& vertices = boxMesh->GetVertices();
    const std::vector<uint32_t>& indices = boxMesh->GetIndices();
    std::vector<Point> wireVertices = std::vector<Point>{vertices.size()};
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        wireVertices[i] = {
            vertices[i].position,
            vertices[i].uv,
            float4(vertices[i].position, 1)
        };
    }
    customBoxMesh = std::make_unique<MeshT<Point>>();
    customBoxMesh->SetVertices(wireVertices);
    customBoxMesh->SetIndices(indices);

    //标准着色器
    shader = std::make_unique<Shader>("Assets/Shader.hlsl");
    //自定义线着色器
    lineShader = std::make_unique<Shader>("Assets/CustomVertexShader.hlsl", GraphicsPreset::DefaultStateLayout, LineMeshLayout);

    //图片纹理
    imageTexture2D = std::make_unique<Texture2D>("Assets/texture.jpg");
    //白色纹理
    whiteTexture2D = std::make_unique<Texture2D>(1);

    //纹理材质球
    material = std::make_unique<Material>(*shader);
    lineMaterial = std::make_unique<Material>(*lineShader);

    //渲染纹理
    renderTexture = std::make_unique<RenderTexture>(
        Graphics::GetDefaultRenderTarget().width,
        Graphics::GetDefaultRenderTarget().height
    );
}

void DeleteAssets()
{
    renderTexture.reset();

    lineMaterial.reset();
    material.reset();

    whiteTexture2D.reset();
    imageTexture2D.reset();

    lineShader.reset();
    shader.reset();

    customBoxMesh.reset();
    boxMesh.reset();
    fullScreenMesh.reset();
}

void Draw(CommandBuffer& commandBuffer, float4 background = float4(0.5, 0.5, 0.5, 1))
{
    //绘制背景色
    commandBuffer.ClearRenderTarget(background);

    //测试变换，纹理
    {
        material->SetTexture(0, *imageTexture2D);
        for (int i = 0; i < 4; ++i)
        {
            float4x4 objectToWorld = float4x4::TRS(
                float3{0, 0, static_cast<float>(i) * 0.1f} + Move[i] * fmod(static_cast<float>(chronograph.Time()) / 6000, 1.0f),
                {-90, 0, 0},
                static_cast<float>(i + 1) / 4.0f
            );

            commandBuffer.Draw(*boxMesh, *material, objectToWorld);
        }
    }

    //测试自定义网格
    lineMaterial->SetTexture(0, *whiteTexture2D);
    commandBuffer.Draw(*customBoxMesh, *lineMaterial, float4x4::TRS(0, 0, 2));

    //测试视口和NDC
    {
        uint32_t halfWidth = commandBuffer.GetCurrentRenderTarget()->width / 2;
        uint32_t halfHeight = commandBuffer.GetCurrentRenderTarget()->height / 2;
        commandBuffer.SetViewProjectionMatrices(float4x4::Identity());
        commandBuffer.SetViewport(static_cast<int32_t>(halfWidth), static_cast<int32_t>(halfHeight), halfWidth / 2, halfHeight / 2);
        material->SetTexture(0, *whiteTexture2D);
        commandBuffer.Draw(*fullScreenMesh, *material, float4x4::Identity());
    }
}

void Update(GLFWwindow* glfwWindow)
{
    //开始计时
    chronograph.Tick();

    //提前申请绘制时使用的子命令缓冲区
    std::vector<CommandBuffer*> commandBuffers = {};
    for (uint32_t i = 0; i < SwapChain::GetGLSwapChain().imageCount; ++i)
        commandBuffers.push_back(&Graphics::ApplyCommandBuffer());

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        //申请开始一次呈现，并获取本次呈现用的主命令缓冲区
        SwapChain::WaitPresentable();
        GLCommandBuffer* presentCommandBuffer;
        if (SwapChain::BeginPresent(&presentCommandBuffer) == false)
            continue; //当前无法呈现

        //取出绘制用的命令缓冲区开始录制绘制命令
        CommandBuffer& commandBuffer = *commandBuffers[SwapChain::GetGLSwapChain().GetCurrentBufferIndex()];
        commandBuffer.BeginRecording();

        //绘制相机1到自定义渲染目标
        commandBuffer.SetRenderTarget(*renderTexture); //设置渲染目标
        commandBuffer.SetViewProjectionMatrices( //设置视角
            inverse(float4x4::TRS({1, 2, -3}, {30, -20, 0}, {1, 1, 1})),
            float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f)
        );
        Draw(commandBuffer, 0);
        commandBuffer.SetRenderTargetToNull();

        //绘制相机2到屏幕
        commandBuffer.SetRenderTarget(Graphics::GetDefaultRenderTarget()); //设置渲染目标
        commandBuffer.ClearRenderTarget();
        commandBuffer.SetViewProjectionMatrices( //设置视角
            inverse(float4x4::TRS({0, 2, 3}, {30, -180, 0}, {1, 1, 1})),
            float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f)
        );
        Draw(commandBuffer);
        //绘制相机1画面到屏幕
        commandBuffer.SetViewport(50, 50, 160, 90);
        commandBuffer.SetViewProjectionMatricesToIdentity();
        material->SetTexture(0, *renderTexture);
        commandBuffer.Draw(*fullScreenMesh, *material, float4x4::Identity());


        //结束绘制命令录制
        commandBuffer.EndRecording();

        //将绘制命令提交到主命令缓冲区
        presentCommandBuffer->ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());
        //将主命令缓冲区提交到管道，完成呈现
        SwapChain::EndPresent(*presentCommandBuffer);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    SwapChain::WaitPresent();

    for (uint32_t i = 0; i < SwapChain::GetGLSwapChain().imageCount; ++i)
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
