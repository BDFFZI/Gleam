#include <thread>
#include <gtest/gtest.h>

#include "LightGraphics/Runtime/GCommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"
#include "LightMath/Runtime/Matrix.hpp"
#include "LightUtility/Runtime/Chronograph.hpp"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "LightMath/Runtime/MatrixMath.hpp"

using namespace Light;

//自定义顶点
struct Vertex
{
    float3 position;
    float2 uv;
    float4 color;
};
GLVertexInput VertexInput = GLVertexInput{
    sizeof(Vertex), {
        GLVertexAttribute{0,offsetof(Vertex, position), VK_FORMAT_R32G32B32_SFLOAT},
        GLVertexAttribute{1,offsetof(Vertex, uv), VK_FORMAT_R32G32_SFLOAT},
        GLVertexAttribute{2,offsetof(Vertex, color), VK_FORMAT_R32G32B32A32_SFLOAT},
    }
};
//三角面网格布局
GLMeshLayout TriangleMeshLayout = {
    VertexInput,
    GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false}
};
//线框条带网格布局
GLMeshLayout LineMeshLayout = {
    VertexInput,
    GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, false}
};
//定义网格
using Mesh = GMeshT<Vertex>;


GraphicsPreset TriangleGraphicsPreset = GraphicsPreset(TriangleMeshLayout);
GraphicsPreset LineGraphicsPreset = GraphicsPreset(LineMeshLayout);

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

std::unique_ptr<GShaderLayout> shaderLayout;
std::unique_ptr<GShader> shader;
std::unique_ptr<GShader> lineShader;

std::unique_ptr<GTexture2D> imageTexture2D;
std::unique_ptr<GTexture2D> whiteTexture2D;

std::unique_ptr<GMaterial> material;
std::unique_ptr<GMaterial> lineMaterial;

std::unique_ptr<GRenderTexture> renderTexture;

void CreateAssets()
{
    //全屏网格
    fullScreenMesh = std::make_unique<Mesh>();
    fullScreenMesh->SetVertices({
        {float3(-1, -1, 1), float2(0, 1), 1},
        {float3(-1, 1, 1), float2(0, 0), 1},
        {float3(1, 1, 1), float2(1, 0), 1},
        {float3(1, -1, 1), float2(1, 1), 1},
    });
    fullScreenMesh->SetIndices({
        0, 1, 2,
        0, 2, 3
    });
    //盒状网格
    RawMesh rawMesh = ModelImporter::ImportObj("Resources/LightGraphicsRuntimeTests/Cube.obj");
    boxMesh = std::make_unique<Mesh>();
    for (size_t i = 0; i < rawMesh.positions.size(); ++i)
    {
        boxMesh->GetVertices().push_back({
            rawMesh.positions[i],
            rawMesh.uvs[i],
            float4(rawMesh.positions[i], 1),
        });
    }
    boxMesh->SetIndices(rawMesh.triangles);

    shaderLayout = std::make_unique<GShaderLayout>(
        std::vector<GLDescriptorBinding>{{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}},
        std::vector<VkPushConstantRange>{{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float4x4)}}
    );
    //标准材质
    shader = std::make_unique<GShader>(*shaderLayout, "Resources/LightGraphicsRuntimeTests/Shader.hlsl");
    material = std::make_unique<GMaterial>(*shaderLayout, shader.get());
    //自定义线材质
    lineShader = std::make_unique<GShader>(*shaderLayout, "Resources/LightGraphicsRuntimeTests/CustomVertexShader.hlsl", LineGraphicsPreset);
    lineMaterial = std::make_unique<GMaterial>(*shaderLayout, lineShader.get());

    //图片纹理
    imageTexture2D = std::make_unique<GTexture2D>("Resources/LightGraphicsRuntimeTests/texture.jpg");
    //白色纹理
    whiteTexture2D = std::make_unique<GTexture2D>(1.0f);

    //渲染纹理
    renderTexture = std::make_unique<GRenderTexture>(
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

    boxMesh.reset();
    fullScreenMesh.reset();
}

void Draw(GCommandBuffer& commandBuffer, float4x4 matrixVP, float4 background = float4(0.5, 0.5, 0.5, 1))
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
            float4x4 matrixMVP = matrixVP * objectToWorld;
            material->SetPushConstant(0, &matrixMVP);

            commandBuffer.DrawMesh(*boxMesh, *material);
        }
    }

    //测试线框材质
    {
        lineMaterial->SetTexture(0, *whiteTexture2D);
        float4x4 matrixMVP = matrixVP * float4x4::TRS(0, 0, 2);
        material->SetPushConstant(0, &matrixMVP);
        commandBuffer.DrawMesh(*boxMesh, *lineMaterial);
    }

    //测试视口和NDC
    {
        uint32_t halfWidth = commandBuffer.GetCurrentRenderTarget()->width / 2;
        uint32_t halfHeight = commandBuffer.GetCurrentRenderTarget()->height / 2;
        commandBuffer.SetViewport(static_cast<int32_t>(halfWidth), static_cast<int32_t>(halfHeight), halfWidth / 2, halfHeight / 2);
        material->SetTexture(0, *whiteTexture2D);
        float4x4 matrixMVP = float4x4::Identity();
        material->SetPushConstant(0, &matrixMVP);
        commandBuffer.DrawMesh(*fullScreenMesh, *material);
    }
}

void Update(GLFWwindow* glfwWindow)
{
    //开始计时
    chronograph.Tick();

    //提前申请绘制时使用的子命令缓冲区
    std::vector<std::unique_ptr<GCommandBuffer>> commandBuffers = {};
    commandBuffers.resize(SwapChain::GetGLSwapChain().imageCount);
    for (auto& commandBuffer : commandBuffers)
        commandBuffer = std::make_unique<GCommandBuffer>();

    while (!glfwWindowShouldClose(glfwWindow))
    {
        glfwPollEvents();

        //申请开始一次呈现，并获取本次呈现用的主命令缓冲区
        SwapChain::WaitPresentable();
        GLCommandBuffer* presentCommandBuffer;
        if (SwapChain::BeginPresent(&presentCommandBuffer) == false)
            continue; //当前无法呈现

        //取出绘制用的命令缓冲区开始录制绘制命令
        GCommandBuffer& commandBuffer = *commandBuffers[SwapChain::GetGLSwapChain().GetCurrentBufferIndex()];
        commandBuffer.BeginRecording();

        //绘制相机1到自定义渲染目标
        commandBuffer.SetRenderTarget(*renderTexture); //设置渲染目标
        Draw(commandBuffer, inverse(float4x4::TRS({1, 2, -3}, {30, -20, 0}, {1, 1, 1})) *
             float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f),
             0);
        commandBuffer.SetRenderTargetToNull();

        //绘制相机2到屏幕
        commandBuffer.SetRenderTarget(Graphics::GetDefaultRenderTarget()); //设置渲染目标
        commandBuffer.ClearRenderTarget();
        Draw(commandBuffer, inverse(float4x4::TRS({0, 2, 3}, {30, -180, 0}, {1, 1, 1})) *
             float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f));

        //绘制相机1画面到屏幕
        commandBuffer.SetViewport(50, 50, 160, 90);
        material->SetTexture(0, *renderTexture);
        float4x4 matrixMVP = float4x4::Identity();
        material->SetPushConstant(0, &matrixMVP);
        commandBuffer.DrawMesh(*fullScreenMesh, *material);

        //结束绘制命令录制
        commandBuffer.EndRecording();

        //将绘制命令提交到主命令缓冲区
        presentCommandBuffer->ExecuteSubCommands(commandBuffer.GetGLCommandBuffer());
        //将主命令缓冲区提交到管道，完成呈现
        SwapChain::EndPresent(*presentCommandBuffer);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    SwapChain::WaitPresent();
}

// TEST(Presentation, Graphics)
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
    Graphics::Initialize(gl, TriangleGraphicsPreset);

    CreateAssets();

    Update(window);

    DeleteAssets();

    Graphics::UnInitialize();
    GL::UnInitialize();
}