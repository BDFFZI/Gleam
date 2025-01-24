#include "RenderingSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightMath/Runtime/MatrixMath.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"
#include "LightRendering/Runtime/Resource/CommandBufferPool.h"

namespace Light
{
    CameraInfo::CameraInfo(Transform& transform, Camera& camera)
    {
        float4x4 matrixV = inverse(float4x4::TRS(transform.localPosition, transform.localRotation, transform.localScale));
        float4x4 matrixP = camera.orthographic
                               ? float4x4::Ortho(camera.halfHeight * camera.aspect, camera.halfHeight, camera.nearClipPlane, camera.farClipPlane)
                               : float4x4::Perspective(camera.fieldOfView, camera.aspect, camera.nearClipPlane, camera.farClipPlane);
        matrixVP = matrixP * matrixV;
        this->camera = &camera;
    }
    bool CameraInfo::operator<(const CameraInfo& other) const
    {
        return other.camera->depth > other.camera->depth;
    }
    RendererInfo::RendererInfo(Transform& transform, Renderer& renderer)
    {
        matrixM = float4x4::TRS(transform.localPosition, transform.localRotation, transform.localScale);
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


    void RenderingSystem::Start()
    {
        vertexColorGSCodeLayout = std::make_unique<GSCodeLayout>("Resources/LightRenderingRuntime/VertexColor.hlsl");
        pointGSInoutLayout = std::make_unique<GSInoutLayout>(
            RenderingConfig::PointMeshLayout, RenderingConfig::ColorFormat, RenderingConfig::DepthStencilFormat);
        lineGSInoutLayout = std::make_unique<GSInoutLayout>(
            RenderingConfig::LineMeshLayout, RenderingConfig::ColorFormat, RenderingConfig::DepthStencilFormat);
        defaultPointShader = std::make_unique<GShader>(*vertexColorGSCodeLayout, *pointGSInoutLayout);
        defaultLineShader = std::make_unique<GShader>(*vertexColorGSCodeLayout, *lineGSInoutLayout);
        defaultPointMaterial = std::make_unique<Material>(*defaultPointShader);
        defaultLineMaterial = std::make_unique<Material>(*defaultLineShader);
    }
    void RenderingSystem::Stop()
    {
        vertexColorGSCodeLayout.reset();
        pointGSInoutLayout.reset();
        lineGSInoutLayout.reset();
        defaultPointShader.reset();
        defaultLineShader.reset();
        defaultPointMaterial.reset();
        defaultLineMaterial.reset();

        CommandBufferPool::Clear();
    }
    
    void RenderingSystem::Update()
    {
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
            GRenderTarget* renderTarget = cameraInfo.camera->renderTarget.value_or(&Graphics::GetDefaultRenderTarget());
            commandBuffer.SetRenderTarget(*renderTarget);
            commandBuffer.ClearRenderTarget(cameraInfo.camera->background);
            commandBuffer.SetViewProjectionMatrices(cameraInfo.matrixVP);
            //绘制每个渲染器
            for (const auto& rendererInfo : rendererInfos)
            {
                commandBuffer.Draw(*rendererInfo.mesh, rendererInfo.matrixM, *rendererInfo.material);
            }
        }
        commandBuffer.EndRecording();
        PresentationSystem->GetPresentGLCommandBuffer().ExecuteSubCommands(commandBuffer);
        CommandBufferPool::Release(commandBuffer);
    }
}