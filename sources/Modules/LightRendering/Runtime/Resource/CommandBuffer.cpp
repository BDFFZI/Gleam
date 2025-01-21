#include "CommandBuffer.h"

#include "LightMath/Runtime/MatrixMath.hpp"

namespace Light
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
        GCommandBuffer::DrawMesh(mesh, material);
    }
}