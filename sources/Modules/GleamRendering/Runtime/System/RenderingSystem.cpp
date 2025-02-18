#include "RenderingSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamRendering/Runtime/CommandBufferPool.h"
#include "GleamRendering/Runtime/Rendering.h"

namespace Gleam
{
    CameraInfo::CameraInfo(Camera& camera, WorldToClip& worldToClip)
        : camera(&camera), worldToClip(&worldToClip)
    {
    }
    bool CameraInfo::operator<(const CameraInfo& other) const
    {
        return other.camera->depth > other.camera->depth;
    }
    RendererInfo::RendererInfo(const float4x4& localToWorld, Renderer& renderer)
        : localToWorld(localToWorld)
    {
        assert(renderer.material.has_value() && renderer.mesh.has_value() && "渲染资源不完整！");

        renderQueue = renderer.material.value()->GetRenderQueue();
        material = renderer.material.value();
        mesh = renderer.mesh.value();
    }
    RendererInfo::RendererInfo(const float4x4& localToWorld, const RenderQueue renderQueue, Material& material, Mesh& mesh, const uint32_t instanceID)
        : localToWorld(localToWorld), renderQueue(renderQueue), material(&material), mesh(&mesh), instanceCount(instanceID)
    {
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
    void RenderingSystem::AddRendererInfo(const RendererInfo& rendererInfo)
    {
        rendererInfos.insert(rendererInfo);
    }

    void RenderingSystem::Start()
    {
        defaultRenderTarget = &SwapChain::GetPresentRenderTarget();
    }
    void RenderingSystem::Update()
    {
        //统计渲染对象
        View<Camera, WorldToClip>::Each([this](auto& camera, auto& cameraTransform)
        {
            cameraInfos.emplace(camera, cameraTransform);
        });
        View<LocalToWorld, Renderer>::Each([this](auto& localToWorld, auto& renderer)
        {
            if (renderer.material.has_value() && renderer.mesh.has_value())
                rendererInfos.emplace(localToWorld.value, renderer);
        });
        //录制渲染命令
        CommandBuffer& commandBuffer = CommandBufferPool::Apply();
        commandBuffer.BeginRecording();
        for (const auto& cameraInfo : cameraInfos)
        {
            commandBuffer.SetWorldInfo(WorldInfo{TimeSystem.GetTime()});
            //设置相机参数
            GRenderTarget* renderTarget = cameraInfo.camera->renderTarget.value_or(defaultRenderTarget);
            commandBuffer.SetRenderTarget(*renderTarget);
            commandBuffer.ClearRenderTarget(cameraInfo.camera->background);
            //绘制每个渲染器
            for (const auto& rendererInfo : rendererInfos)
            {
                commandBuffer.SetObjectInfo(ObjectInfo{mul(cameraInfo.worldToClip->value, rendererInfo.localToWorld)});
                commandBuffer.Draw(*rendererInfo.mesh, *rendererInfo.material, rendererInfo.instanceCount);
            }
        }
        commandBuffer.EndRecording();
        //执行渲染命令
        PresentationSystem.GetPresentGLCommandBuffer().ExecuteSubCommands(commandBuffer);
        //清除渲染资源
        CommandBufferPool::Release(commandBuffer);
        cameraInfos.clear();
        rendererInfos.clear();
    }
}