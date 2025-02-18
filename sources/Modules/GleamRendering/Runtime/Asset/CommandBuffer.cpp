#include "CommandBuffer.h"

#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Rendering.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    void CommandBuffer::SetWorldInfo(const WorldInfo& worldInfo) const
    {
        worldInfoBuffer.SetData(&worldInfo);
    }
    void CommandBuffer::SetObjectInfo(const ObjectInfo& objectInfo)
    {
        objectInfoBuffer = objectInfo;
    }

    void CommandBuffer::Draw(GMesh& mesh, GMaterial& material, const uint32_t instanceCount)
    {
        material.SetBuffer(0, worldInfoBuffer);
        material.SetPushConstant(0, &objectInfoBuffer);
        DrawMesh(mesh, material, "", instanceCount);
    }
    void CommandBuffer::Blit(GTexture* source, GRenderTarget* destination)
    {
        SetRenderTarget(*destination);
        SetObjectInfo(ObjectInfo{float4x4::Identity()});
        Rendering::GetBlitMaterial()->SetAlbedoTex(*source);
        Draw(*Rendering::GetFullScreenMesh(), *Rendering::GetBlitMaterial());
    }
}