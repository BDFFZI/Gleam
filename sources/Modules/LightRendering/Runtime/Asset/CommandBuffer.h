#pragma once
#include "LightGraphics/Runtime/GCommandBuffer.h"

namespace Light
{
    class CommandBuffer : public GCommandBuffer
    {
    public:
        void EndRecording() override;

        void SetViewProjectionMatrices(const float4x4& viewMatrix, const float4x4& projMatrix);
        void SetViewProjectionMatrices(const float4x4& matrixVP);
        void SetViewProjectionMatricesToIdentity();

        void Draw(GMesh& mesh, const float4x4& modelMatrix, GMaterial& material);
        void Blit(GTexture* source, GRenderTarget* destination);

    private:
        float4x4 matrixVP = float4x4::Identity();
    };
}