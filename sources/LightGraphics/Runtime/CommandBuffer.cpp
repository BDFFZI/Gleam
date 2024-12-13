#include "CommandBuffer.h"

#include <cassert>

#include "LightMath/Runtime/MatrixMath.hpp"

using namespace Light;

void CommandBuffer::BeginRecording()
{
    glCommandBuffer.BeginRecording();
}
void CommandBuffer::EndRecording()
{
    if (currentRenderTarget != nullptr)
        EndRendering();

    glCommandBuffer.EndRecording();
    //全部属性重置为初始值
    currentMesh = nullptr;
    currentMaterial = nullptr;
    currentRenderTarget = nullptr;
    matrixVP = float4x4::Identity();
}

void CommandBuffer::BeginRendering(const RenderTargetAsset& renderTarget, const bool clearColor)
{
    if (currentRenderTarget != &renderTarget)
    {
        glCommandBuffer.BeginRendering(
            VkRect2D{
                {0, 0}, {
                    renderTarget.width,
                    renderTarget.height
                }
            }, clearColor,
            *renderTarget.glColorImageView,
            renderTarget.glDepthStencilImageView,
            renderTarget.glColorResolveImageView
        );
        currentRenderTarget = &renderTarget;
    }

    SetViewportToFullscreen();
}
void CommandBuffer::EndRendering()
{
    currentRenderTarget = nullptr;
    glCommandBuffer.EndRendering();
}

void CommandBuffer::SetRenderTarget(const RenderTargetAsset& renderTarget)
{
    if (currentRenderTarget != &renderTarget && currentRenderTarget != nullptr)
        EndRendering();
    BeginRendering(renderTarget);
}
void CommandBuffer::SetRenderTargetToNull()
{
    EndRendering();
}

void CommandBuffer::SetViewport(const int32_t x, const int32_t y, const uint32_t width, const uint32_t height) const
{
    assert(x >=0 && static_cast<uint32_t>(x) < currentRenderTarget->width && "x超出最大渲染范围！");
    assert(y >=0 && static_cast<uint32_t>(y) < currentRenderTarget->height && "y超出最大渲染范围！");
    assert(x + width <= currentRenderTarget->width && "width超出最大渲染范围！");
    assert(y + height <= currentRenderTarget->height && "height超出最大渲染范围！");

    int32_t rightY = static_cast<int32_t>(currentRenderTarget->height - (y + height)); //转换到右手坐标系

    glCommandBuffer.SetViewport(
        static_cast<float>(x), static_cast<float>(rightY + height),
        static_cast<float>(width), -static_cast<float>(height)
    ); //利用-height的特性，实现剪辑空间的上下反转。这样传入左手坐标系的顶点后就可以负负得正了。
    glCommandBuffer.SetScissor(
        {x, rightY},
        {width, height}
    );
}
void CommandBuffer::SetViewportToFullscreen() const
{
    SetViewport(0, 0, currentRenderTarget->width, currentRenderTarget->height);
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

void CommandBuffer::Draw(MeshAsset& mesh, MaterialAsset& material)
{
    mesh.BindToPipeline(glCommandBuffer, currentMesh);
    material.shaderAsset->BindToPipeline(glCommandBuffer, currentShader);
    material.BindToPipeline(glCommandBuffer, currentMaterial);

    currentMesh = &mesh;
    currentShader = material.shaderAsset;
    currentMaterial = &material;

    glCommandBuffer.DrawIndexed(mesh.glIndexCount);
}
void CommandBuffer::Draw(MeshAsset& mesh, Material& material, const float4x4& modelMatrix)
{
    float4x4 matrixMVP = mul(matrixVP, modelMatrix);
    material.SetPushConstant(0, &matrixMVP);
    Draw(mesh, material);
}
void CommandBuffer::ClearRenderTarget(const std::optional<float4>& color, const std::optional<float>& depth) const
{
    VkClearColorValue colorValue;
    if (color.has_value())
        std::memcpy(colorValue.float32, color.value().data, sizeof(float4));
    VkClearDepthStencilValue depthStencilValue;
    if (depth.has_value())
    {
        depthStencilValue.depth = depth.value();
        depthStencilValue.stencil = 0;
    }

    glCommandBuffer.ClearAttachments(
        VkRect2D{{0, 0}, {currentRenderTarget->width, currentRenderTarget->height}},
        color.has_value() ? std::optional(colorValue) : std::nullopt,
        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        depth.has_value() ? std::optional(depthStencilValue) : std::nullopt
    );
}