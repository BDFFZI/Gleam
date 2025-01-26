#include <thread>
#include <gtest/gtest.h>

#include "LightGraphics/Runtime/GCommandBuffer.h"
#include "LightGraphics/Runtime/Graphics.h"
#include "LightImport/Runtime/ShaderImporter.h"
#include "LightMath/Runtime/Matrix.h"
#include "LightUtility/Runtime/Timer.h"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightGraphics/Runtime/Resource/GRenderTarget/GRenderTexture.h"
#include "LightMath/Runtime/MatrixMath.h"

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
//定义网格
using Mesh = GMeshT<Vertex>;

//变换测试用的移动方向
inline float3 Move[4] = {
    float3(-1, 1, 1),
    float3(1, 1, -1),
    float3(1, -1, 1),
    float3(-1, -1, -1),
};

//计时器，每帧置零
Timer chronograph;

std::unique_ptr<Mesh> fullScreenMesh;
std::unique_ptr<Mesh> cubeMesh;
std::unique_ptr<GShader> shader;
std::unique_ptr<GShader> lineShader;
std::unique_ptr<GTexture2D> imageTexture2D;
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
    cubeMesh = std::make_unique<Mesh>();
    for (size_t i = 0; i < rawMesh.positions.size(); ++i)
    {
        cubeMesh->GetVertices().push_back({
            rawMesh.positions[i],
            rawMesh.uvs[i],
            float4(rawMesh.positions[i], 1),
        });
    }
    cubeMesh->SetIndices(rawMesh.triangles);
    //标准材质
    shader = std::make_unique<GShader>(GSCodeLayout{"Resources/LightGraphicsRuntimeTests/shader.hlsl"});
    material = std::make_unique<GMaterial>(*shader);
    //自定义线材质
    GSInoutLayout LineGSInoutLayout = GSInoutLayout{
        GLMeshLayout{VertexInput, GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_LINE_STRIP, false}},
        Graphics::GetGraphicsConfig().defaultGSInoutLayout.colorFormat,
        Graphics::GetGraphicsConfig().defaultGSInoutLayout.depthStencilFormat
    };
    lineShader = std::make_unique<GShader>(
        GSCodeLayout{"Resources/LightGraphicsRuntimeTests/shader.hlsl"},
        Graphics::GetGraphicsConfig().defaultGSStateLayout, Graphics::GetGraphicsConfig().defaultGSAssetLayout, LineGSInoutLayout);
    lineMaterial = std::make_unique<GMaterial>(*lineShader);
    //图片纹理
    imageTexture2D = std::make_unique<GTexture2D>("Resources/LightGraphicsRuntimeTests/texture.jpg");

    //渲染纹理
    renderTexture = std::make_unique<GRenderTexture>(
        SwapChain::GetPresentRenderTarget().width,
        SwapChain::GetPresentRenderTarget().height
    );
}

void DeleteAssets()
{
    renderTexture.reset();

    imageTexture2D.reset();

    lineShader.reset();
    shader.reset();
    lineMaterial.reset();
    material.reset();

    cubeMesh.reset();
    fullScreenMesh.reset();
}

void TestBackground(GCommandBuffer& commandBuffer)
{
    //绘制背景色
    commandBuffer.ClearRenderTarget(float4(0.5, 0.5, 0.5, 1));
}
void TestViewport(GCommandBuffer& commandBuffer)
{
    commandBuffer.SetViewport(50, 50, 100, 50);
    commandBuffer.ClearRenderTarget(float4(0.5, 0, 0, 1));
    commandBuffer.SetViewportToFullscreen();
}
void TestMaterialAndCubeMesh(GCommandBuffer& commandBuffer, float4x4 matrixVP)
{
    for (int i = 0; i < 4; ++i)
    {
        float4x4 matrixM = float4x4::TRS(
            float3{0, 0, static_cast<float>(i) * 0.1f} + Move[i] * fmod(static_cast<float>(chronograph.Time()) / 6000, 1.0f),
            {-90, 0, 0},
            static_cast<float>(i + 1) / 4.0f
        );

        float4x4 matrixMVP = mul(matrixVP, matrixM);
        material->SetTexture(0, *imageTexture2D);
        material->SetPushConstant(0, &matrixMVP);
        commandBuffer.DrawMesh(*cubeMesh, *material);
        material->SetTexture(0, Graphics::GetDefaultTexture2D());
    }
}
void TestWireCubeMesh(GCommandBuffer& commandBuffer, float4x4 matrixVP)
{
    float4x4 matrixMVP = mul(matrixVP, float4x4::TRS(0, 0, 2));
    lineMaterial->SetPushConstant(0, &matrixMVP);
    commandBuffer.DrawMesh(*cubeMesh, *lineMaterial);
}
void TestRenderTarget(GCommandBuffer& commandBuffer, float4x4 matrixVP)
{
    commandBuffer.SetRenderTarget(*renderTexture);
    TestMaterialAndCubeMesh(commandBuffer, matrixVP);

    commandBuffer.SetRenderTarget(SwapChain::GetPresentRenderTarget());
    commandBuffer.SetViewport(50, 110, 100, 50);
    material->SetTexture(0, *renderTexture);

    float4x4 matrixMVP = float4x4::Identity();
    material->SetPushConstant(0, &matrixMVP);
    commandBuffer.DrawMesh(*fullScreenMesh, *material);

    material->SetTexture(0, Graphics::GetDefaultTexture2D());
    commandBuffer.SetViewportToFullscreen();
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

        float4x4 matrixP = float4x4::Perspective(60, 16.0f / 9.0f, 0.3f, 1000.0f);
        float4x4 matrixV = inverse(float4x4::TRS({1, 2, -3}, {30, -20, 0}, {1, 1, 1}));
        float4x4 matrixVP = mul(matrixP, matrixV);

        commandBuffer.SetRenderTarget(SwapChain::GetPresentRenderTarget());
        TestBackground(commandBuffer);
        TestMaterialAndCubeMesh(commandBuffer, matrixVP);
        TestWireCubeMesh(commandBuffer, matrixVP);
        TestViewport(commandBuffer);
        TestRenderTarget(commandBuffer, matrixVP);

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

    std::unique_ptr<GraphicsConfig> graphicsConfig = std::make_unique<GraphicsConfig>();
    graphicsConfig->defaultGSInoutLayout = GSInoutLayout{
        GLMeshLayout{VertexInput, GLInputAssembly{VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false}},
        graphicsConfig->presentColorFormat, graphicsConfig->presentDepthStencilFormat
    };
    graphicsConfig->defaultGSAssetLayout = GSAssetLayout{
        std::vector<GLDescriptorBinding>{{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}},
        std::vector<VkPushConstantRange>{{VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float4x4)}}
    };
    Graphics::Initialize(gl, std::move(graphicsConfig));

    CreateAssets();

    Update(window);

    DeleteAssets();

    Graphics::UnInitialize();
    GL::UnInitialize();
}