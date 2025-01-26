#include "RenderingSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightMath/Runtime/MatrixMath.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Resource/CommandBufferPool.h"
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

    CameraInfo::CameraInfo(Transform& transform, Camera& camera)
    {
        float4x4 matrixV = transform.GetWorldToLocalMatrix();
        float4x4 matrixP = camera.orthographic
                               ? float4x4::Ortho(camera.halfHeight * camera.aspect, camera.halfHeight, camera.nearClipPlane, camera.farClipPlane)
                               : float4x4::Perspective(camera.fieldOfView, camera.aspect, camera.nearClipPlane, camera.farClipPlane);
        matrixVP = mul(matrixP, matrixV);
        this->camera = &camera;
    }
    bool CameraInfo::operator<(const CameraInfo& other) const
    {
        return other.camera->depth > other.camera->depth;
    }
    RendererInfo::RendererInfo(Transform& transform, Renderer& renderer)
    {
        matrixM = transform.GetLocalToWorldMatrix();
        renderQueue = renderer.material->GetRenderQueue();
        material = renderer.material;
        mesh = renderer.mesh;
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
    const std::unique_ptr<GRenderTexture>& RenderingSystem::GetDefaultRenderTarget() const
    {
        return defaultRenderTarget;
    }

    void RenderingSystem::Start()
    {
        defaultRenderTarget = std::make_unique<GRenderTexture>(Window->GetResolution());
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
        defaultRenderTarget.reset();
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
        //更新默认渲染目标
        if (any(defaultRenderTarget->GetSize() != Window->GetResolution()))
            defaultRenderTarget = std::make_unique<GRenderTexture>(Window->GetResolution());

        //统计渲染对象
        cameraInfos.clear();
        View<Transform, Camera>::Each([this](auto& transform, auto& camera)
        {
            cameraInfos.emplace(transform, camera);
        });
        rendererInfos.clear();
        View<Transform, Renderer>::Each([this](auto& transform, auto& renderer)
        {
            rendererInfos.emplace(transform, renderer);
        });
        //渲染
        CommandBuffer& commandBuffer = CommandBufferPool::Apply();
        commandBuffer.BeginRecording();
        for (const auto& cameraInfo : cameraInfos)
        {
            //设置相机参数
            GRenderTarget* renderTarget = cameraInfo.camera->renderTarget.value_or(defaultRenderTarget.get());
            commandBuffer.SetRenderTarget(*renderTarget);
            commandBuffer.ClearRenderTarget(cameraInfo.camera->background);
            commandBuffer.SetViewProjectionMatrices(cameraInfo.matrixVP);
            //绘制每个渲染器
            for (const auto& rendererInfo : rendererInfos)
            {
                commandBuffer.Draw(*rendererInfo.mesh, rendererInfo.matrixM, *rendererInfo.material);
            }
        }
        //将画面复制到呈现缓冲区
        commandBuffer.Blit(defaultRenderTarget.get(), &SwapChain::GetPresentRenderTarget());

        commandBuffer.EndRecording();
        PresentationSystem->GetPresentGLCommandBuffer().ExecuteSubCommands(commandBuffer);
        CommandBufferPool::Release(commandBuffer);
    }
}