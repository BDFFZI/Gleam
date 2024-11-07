#include "CommandBuffer.h"

#include "LightMath/Runtime/MatrixMath.hpp"

using namespace Light;

GLCommandBuffer& CommandBuffer::GetGLCommandBuffer()
{
    return glCommandBuffer;
}
void CommandBuffer::BeginRecording()
{
    glCommandBuffer.BeginRecording();
}
void CommandBuffer::EndRecording()
{
    glCommandBuffer.EndRecording();
    lastMesh = nullptr;
    lastMaterial = nullptr;
    lastRenderTexture = nullptr;
    matrixVP = {};
}

void CommandBuffer::BeginRendering(const RenderTextureBase& renderTarget, const bool clearColor)
{
    glCommandBuffer.BeginRendering(
        VkRect2D{
            {0, 0}, {
                renderTarget.GetWidth(),
                renderTarget.GetHeight()
            }
        }, clearColor,
        renderTarget.GetGLColorImageView(),
        renderTarget.GetGLDepthStencilImageView(),
        renderTarget.GetGLColorResolveImageView()
    );
    // glCommandBuffer.SetRasterizationSamples(renderTarget.GetSampleCount());

    lastRenderTexture = &renderTarget;
}
void CommandBuffer::EndRendering() const
{
    glCommandBuffer.EndRendering();
}

void CommandBuffer::SetViewport(const int32_t x, const int32_t y, const uint32_t width, const uint32_t height) const
{
    glCommandBuffer.SetViewport(
        static_cast<float>(x), static_cast<float>(y + height),
        static_cast<float>(width), -static_cast<float>(height)
    ); //利用-height的特性，实现剪辑空间的上下反转。这样传入左手坐标系的顶点后就可以负负得正了。
    glCommandBuffer.SetScissor(
        {x, y},
        {width, height}
    );
}
void CommandBuffer::SetViewProjectionMatrices(const float4x4& viewMatrix, const float4x4& projMatrix)
{
    matrixVP = mul(projMatrix, viewMatrix);
}

void CommandBuffer::Draw(MeshBase* mesh, const float4x4& modelMatrix, MaterialBase* material)
{
    //绑定网格
    if (mesh != lastMesh)
    {
        mesh->BindToPipeline(glCommandBuffer, lastMesh);
        lastMesh = mesh;
    }

    //绑定材质球
    if (material != lastMaterial)
    {
        material->BindToPipeline(glCommandBuffer, lastMaterial);
        lastMaterial = material;
    }

    //推送常量
    BuiltInPushConstant pushConstant = {mul(matrixVP, modelMatrix)};
    glCommandBuffer.PushConstant(
        material->GetShader()->GetGLPipelineLayout(),
        material->GetShader()->GetPushConstantBinding()[0],
        &pushConstant);

    glCommandBuffer.DrawIndexed(mesh->GetIndexCount());
}
void CommandBuffer::ClearRenderTexture(float4 color, const float depth) const
{
    glCommandBuffer.ClearAttachments(
        VkRect2D{{0, 0}, {lastRenderTexture->GetWidth(), lastRenderTexture->GetHeight()}},
        color.data, depth, 0
    );
}
