#include "RenderingSystem.h"

#include "GleamEngine/Editor/System/EditorTimeSystem.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamGraphics/Runtime/SwapChain.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Entity/Camera.h"
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
        assert(!renderer.material.expired() && !renderer.mesh.expired() && "渲染资源不完整！");

        renderQueue = renderer.material.lock()->GetRenderQueue();
        material = renderer.material.lock().get();
        mesh = renderer.mesh.lock().get();
    }
    RendererInfo::RendererInfo(const float4x4& localToWorld, const RenderQueue renderQueue, Material& material, Mesh& mesh, const uint32_t instanceID)
        : localToWorld(localToWorld), renderQueue(renderQueue), material(&material), mesh(&mesh), instanceCount(instanceID)
    {
    }
    bool RendererInfo::operator<(const RendererInfo& other) const
    {
        return renderQueue < other.renderQueue;
    }

    GRenderTarget& RenderingSystem::GetDefaultRenderTarget() const
    {
        return *defaultRenderTarget;
    }
    void RenderingSystem::SetDefaultRenderTarget(GRenderTarget& renderTarget)
    {
        defaultRenderTarget = &renderTarget;
    }
    void RenderingSystem::AddRendererInfo(const RendererInfo& rendererInfo)
    {
        rendererInfos.insert(rendererInfo);
    }
    void RenderingSystem::AddRendererInfos(const std::vector<RendererInfo>& rendererInfos)
    {
        this->rendererInfos.insert(rendererInfos.begin(), rendererInfos.end());
    }

    void RenderingSystem::Start()
    {
        defaultRenderTarget = &SwapChain::GetPresentRenderTarget();
    }
    void RenderingSystem::Update()
    {
        TimeSystem& timeSystem = World::GetSystemAllocator().HasSystem<TimeSystem>()
                                     ? World::GetSystemAllocator().GetSystem<TimeSystem>()
                                     : World::GetSystemAllocator().GetSystem<EditorTimeSystem>();
        PresentationSystem& presentationSystem = World::GetSystemAllocator().GetSystem<PresentationSystem>();

        //统计渲染对象
        World::GetView<Camera, WorldToClip>().Each([this](auto& camera, auto& cameraTransform)
        {
            cameraInfos.emplace(camera, cameraTransform);
        });
        World::GetView<LocalToWorld, Renderer>().Each([this](auto& localToWorld, auto& renderer)
        {
            if (!renderer.material.expired() && !renderer.mesh.expired() && renderer.mesh.lock()->GetGLIndexCount() != 0)
                rendererInfos.emplace(localToWorld.value, renderer);
        });
        //录制渲染命令
        CommandBuffer& commandBuffer = CommandBufferPool::Apply();
        commandBuffer.BeginRecording();
        for (const auto& cameraInfo : cameraInfos)
        {
            commandBuffer.SetWorldInfo(WorldInfo{timeSystem.GetTime()});
            //设置相机参数
            GRenderTarget& renderTarget = cameraInfo.camera->renderTarget.value_or(*defaultRenderTarget);
            commandBuffer.SetRenderTarget(renderTarget);
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
        presentationSystem.GetPresentGLCommandBuffer().ExecuteSubCommands(commandBuffer);
        //清除渲染资源
        CommandBufferPool::Release(commandBuffer);
        cameraInfos.clear();
        rendererInfos.clear();
    }
}