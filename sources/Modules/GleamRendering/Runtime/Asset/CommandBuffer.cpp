#include "CommandBuffer.h"

#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Runtime/Rendering.h"
#include "GleamRendering/Runtime/System/RenderingSystem.h"

namespace Gleam
{
    void CommandBuffer::EndRecording()
    {
        GCommandBuffer::EndRecording();
        matrixVP = float4x4::Identity();
    }
    void CommandBuffer::SetViewProjectionMatrices(const float4x4& viewMatrix, const float4x4& projMatrix)
    {
        matrixVP = mul(projMatrix, viewMatrix);
    }
    void CommandBuffer::SetViewProjectionMatrices(const float4x4& matrixVP)
    {
        this->matrixVP = matrixVP;
    }
    void CommandBuffer::SetViewProjectionMatricesToIdentity()
    {
        SetViewProjectionMatrices(float4x4::Identity());
    }

    void CommandBuffer::Draw(GMesh& mesh, const float4x4& modelMatrix, GMaterial& material)
    {
        float4x4 matrixMVP = mul(matrixVP, modelMatrix);
        material.SetPushConstant(0, &matrixMVP);
        DrawMesh(mesh, material);
    }
    void CommandBuffer::Blit(GTexture* source, GRenderTarget* destination)
    {
        SetRenderTarget(*destination);
        SetViewProjectionMatricesToIdentity();
        Rendering::GetBlitMaterial()->SetTexture(0, *source);
        Draw(*Rendering::GetFullScreenMesh(), float4x4::Identity(), *Rendering::GetBlitMaterial());
    }
}