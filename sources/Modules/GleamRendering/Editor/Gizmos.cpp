#include "Gizmos.h"

#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"

namespace Gleam
{
    Gizmos& Gizmos::GetInstance()
    {
        static Gizmos instance = Engine::CreateSystem<Gizmos>();
        return instance;
    }
    float4x4& Gizmos::LocalToWorld()
    {
        return localToWorld;
    }
    void Gizmos::DrawCuboid(Cuboid cuboid)
    {
        // RenderingSystem.AddRendererInfo(RendererInfo{
        //    mul(localToWorld,float4x4::Translate(float3{}), RenderQueue_PostProcessing, *lineMeshMaterial, *cubeMesh
        // });
    }
}