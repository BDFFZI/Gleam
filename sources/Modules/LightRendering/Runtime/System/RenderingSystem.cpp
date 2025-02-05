#include "RenderingSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Asset/CommandBufferPool.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    std::unique_ptr<Mesh> CreateFullScreenMesh()
    {
        std::unique_ptr<Mesh> fullScreenMesh = std::make_unique<Mesh>();
        fullScreenMesh->SetPositions(std::vector{
            float3(-1, -1, 1),
            float3(-1, 1, 1),
            float3(1, 1, 1),
            float3(1, -1, 1),
        });
        fullScreenMesh->SetUVs({
            float2(0, 1),
            float2(0, 0),
            float2(1, 0),
            float2(1, 1)
        });
        fullScreenMesh->SetIndices({
            0, 1, 2,
            0, 2, 3
        });
        return fullScreenMesh;
    }

    CameraInfo::CameraInfo(Camera& camera, WorldToClip& cameraTransform)
        : camera(&camera), cameraTransform(&cameraTransform)
    {
    }
    bool CameraInfo::operator<(const CameraInfo& other) const
    {
        return other.camera->depth > other.camera->depth;
    }
    RendererInfo::RendererInfo(const float4x4& localToWorld, Renderer& renderer)
        : localToWorld(localToWorld)
    {
        renderQueue = renderer.material->GetRenderQueue();
        material = renderer.material;
        mesh = renderer.mesh;
    }
    bool RendererInfo::operator<(const RendererInfo& other) const
    {
        return renderQueue < other.renderQueue;
    }

    const std::unique_ptr<Material>& RenderingSystem::GetDefaultPointMaterial() const
    {
        return defaultPointMaterial;
    }
    const std::unique_ptr<Material>& RenderingSystem::GetDefaultLineMaterial() const
    {
        return defaultLineMaterial;
    }
    const std::unique_ptr<Mesh>& RenderingSystem::GetFullScreenMesh() const
    {
        return fullScreenMesh;
    }
    const std::unique_ptr<Material>& RenderingSystem::GetBlitMaterial() const
    {
        return blitMaterial;
    }
    GRenderTarget* RenderingSystem::GetDefaultRenderTarget() const
    {
        return defaultRenderTarget;
    }

    void RenderingSystem::SetDefaultRenderTarget(GRenderTarget* renderTarget)
    {
        defaultRenderTarget = renderTarget;
    }

    void RenderingSystem::Start()
    {
        defaultRenderTarget = &SwapChain::GetPresentRenderTarget();
        //顶点绘制
        GSCodeLayout vertexColorGSCodeLayout = GSCodeLayout("Resources/LightRenderingRuntime/VertexColor.hlsl");
        GSInoutLayout pointGSInoutLayout = GSInoutLayout(
            RenderingConfig::PointMeshLayout, RenderingConfig::ColorFormat, RenderingConfig::DepthStencilFormat);
        GSInoutLayout lineGSInoutLayout = GSInoutLayout(
            RenderingConfig::LineMeshLayout, RenderingConfig::ColorFormat, RenderingConfig::DepthStencilFormat);
        defaultPointShader = std::make_unique<GShader>(vertexColorGSCodeLayout, pointGSInoutLayout);
        defaultLineShader = std::make_unique<GShader>(vertexColorGSCodeLayout, lineGSInoutLayout);
        defaultPointMaterial = std::make_unique<Material>(*defaultPointShader);
        defaultLineMaterial = std::make_unique<Material>(*defaultLineShader);
        //位块传输
        fullScreenMesh = CreateFullScreenMesh();
        blitShader = std::make_unique<GShader>(GSCodeLayout("Resources/LightRenderingRuntime/Blit.hlsl"));
        blitMaterial = std::make_unique<Material>(*blitShader);
    }
    void RenderingSystem::Stop()
    {
        defaultPointShader.reset();
        defaultLineShader.reset();
        defaultPointMaterial.reset();
        defaultLineMaterial.reset();
        fullScreenMesh.reset();
        blitShader.reset();
        blitMaterial.reset();

        CommandBufferPool::Clear();
    }
    void RenderingSystem::Update()
    {
        //统计渲染对象
        cameraInfos.clear();
        View<Camera, WorldToClip>::Each([this](auto& camera, auto& cameraTransform)
        {
            cameraInfos.emplace(camera, cameraTransform);
        });
        rendererInfos.clear();
        View<LocalToWorld, Renderer>::Each([this](auto& localToWorld, auto& renderer)
        {
            rendererInfos.emplace(localToWorld.value, renderer);
        });
        //渲染
        CommandBuffer& commandBuffer = CommandBufferPool::Apply();
        commandBuffer.BeginRecording();
        for (const auto& cameraInfo : cameraInfos)
        {
            //设置相机参数
            GRenderTarget* renderTarget = cameraInfo.camera->renderTarget.value_or(defaultRenderTarget);
            commandBuffer.SetRenderTarget(*renderTarget);
            commandBuffer.ClearRenderTarget(cameraInfo.camera->background);
            commandBuffer.SetViewProjectionMatrices(cameraInfo.cameraTransform->value);
            //绘制每个渲染器
            for (const auto& rendererInfo : rendererInfos)
            {
                commandBuffer.Draw(*rendererInfo.mesh, rendererInfo.localToWorld, *rendererInfo.material);
            }
        }
        commandBuffer.EndRecording();
        //执行渲染命令
        PresentationSystem->GetPresentGLCommandBuffer().ExecuteSubCommands(commandBuffer);
        CommandBufferPool::Release(commandBuffer);
    }
}