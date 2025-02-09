#include "RenderingSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightGraphics/Runtime/SwapChain.h"
#include "LightRendering/Runtime/Component/Camera.h"
#include "LightRendering/Runtime/CommandBufferPool.h"
#include "LightRendering/Runtime/Rendering.h"

namespace Light
{
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
        renderQueue = renderer.material.value()->GetRenderQueue();
        material = renderer.material.value();
        mesh = renderer.mesh.value();
    }
    bool RendererInfo::operator<(const RendererInfo& other) const
    {
        return renderQueue < other.renderQueue;
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
            if (renderer.material.has_value() && renderer.mesh.has_value())
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