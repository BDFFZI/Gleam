#include "GizmosSystem.h"

#include "GleamECS/Runtime/World.h"
#include "GleamRendering/Editor/Gizmos.h"

namespace Gleam
{
    GizmosSystem& GizmosSystem::GetInstance()
    {
        static GizmosSystem instance = Engine::CreateSystem<GizmosSystem>();
        return instance;
    }

    void GizmosSystem::Start()
    {
        postProcessSystem.OnUpdate() = []
        {
            //清空绘制队列
            Gizmos::cuboidQueue.Clear();
            Gizmos::sphereQueue.Clear();
            Gizmos::wireCuboidQueue.Clear();
            Gizmos::wireSphereQueue.Clear();
            Gizmos::pointsMesh->SetVertices({});
            Gizmos::pointsMesh->SetIndices({});
            Gizmos::linesMesh->SetVertices({});
            Gizmos::linesMesh->SetIndices({});
        };
        World::AddSystem(postProcessSystem);
    }
    void GizmosSystem::Stop()
    {
        World::RemoveSystem(postProcessSystem);
    }
    void GizmosSystem::Update()
    {
        Gizmos::Draw(Gizmos::cuboidQueue, *Gizmos::cubeMesh);
        Gizmos::Draw(Gizmos::sphereQueue, *Gizmos::sphereMesh);
        Gizmos::Draw(Gizmos::wireCuboidQueue, *Gizmos::cubeMesh);
        Gizmos::Draw(Gizmos::wireSphereQueue, *Gizmos::sphereMesh);
        if (!Gizmos::pointsMesh->GetIndices().empty())
        {
            Gizmos::pointsMesh->SetDirty();
            GlobalRenderingSystem.AddRendererInfo(RendererInfo{
                float4x4::Identity(), RenderQueue_Opaque, *Gizmos::pointsMaterial, *Gizmos::pointsMesh
            });
        }
        if (!Gizmos::linesMesh->GetIndices().empty())
        {
            Gizmos::linesMesh->SetDirty();
            GlobalRenderingSystem.AddRendererInfo(RendererInfo{
                float4x4::Identity(), RenderQueue_Opaque, *Gizmos::linesMaterial, *Gizmos::linesMesh
            });
        }
    }
}