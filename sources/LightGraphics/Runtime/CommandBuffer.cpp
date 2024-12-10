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
    if (lastRenderTarget != nullptr)
        EndRendering();

    glCommandBuffer.EndRecording();
    //全部属性重置为初始值
    lastMesh = nullptr;
    lastMaterial = nullptr;
    lastRenderTarget = nullptr;
    matrixVP = float4x4::Identity();
}

void CommandBuffer::BeginRendering(const RenderTargetBase& renderTarget, const bool clearColor)
{
    if (lastRenderTarget != &renderTarget)
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
        lastRenderTarget = &renderTarget;
    }

    SetViewportToFullscreen();
}
void CommandBuffer::EndRendering()
{
    lastRenderTarget = nullptr;
    glCommandBuffer.EndRendering();
}

void CommandBuffer::SetRenderTarget(const RenderTargetBase& renderTarget)
{
    if (lastRenderTarget != &renderTarget && lastRenderTarget != nullptr)
        EndRendering();
    BeginRendering(renderTarget);
}
void CommandBuffer::SetRenderTargetToNull()
{
    EndRendering();
}

void CommandBuffer::SetViewport(const int32_t x, const int32_t y, const uint32_t width, const uint32_t height) const
{
    assert(x >=0 && static_cast<uint32_t>(x) < lastRenderTarget->GetWidth() && "x超出最大渲染范围！");
    assert(y >=0 && static_cast<uint32_t>(y) < lastRenderTarget->GetHeight() && "y超出最大渲染范围！");
    assert(x + width <= lastRenderTarget->GetWidth() && "width超出最大渲染范围！");
    assert(y + height <= lastRenderTarget->GetHeight() && "height超出最大渲染范围！");

    int32_t rightY = static_cast<int32_t>(lastRenderTarget->GetHeight() - (y + height)); //转换到右手坐标系

    glCommandBuffer.SetViewport(
        static_cast<float>(x), static_cast<float>(rightY + height),
        static_cast<float>(width), -static_cast<float>(height)
    ); //利用-height的特性，实现剪辑空间的上下反转。这样传入左手坐标系的顶点后就可以负负得正了。
    glCommandBuffer.SetScissor(
        {x, rightY},
        {width, height}
    );
}
void CommandBuffer::SetViewProjectionMatrices(const float4x4& viewMatrix, const float4x4& projMatrix)
{
    matrixVP = mul(projMatrix, viewMatrix);
}
void CommandBuffer::SetViewProjectionMatrices(const float4x4& matrixVP)
{
    this->matrixVP = matrixVP;
}

void CommandBuffer::SetViewportToFullscreen() const
{
    SetViewport(0, 0, lastRenderTarget->GetWidth(), lastRenderTarget->GetHeight());
}
void CommandBuffer::SetViewProjectionMatricesToIdentity()
{
    SetViewProjectionMatrices(float4x4::Identity());
}

void CommandBuffer::Draw(MeshBase& mesh, MaterialBase& material, const float4x4& modelMatrix)
{
    //绑定网格
    if (&mesh != lastMesh)
    {
        mesh.BindToPipeline(glCommandBuffer, lastMesh);
        lastMesh = &mesh;
    }

    //绑定材质球
    if (&material != lastMaterial)
    {
        material.BindToPipeline(glCommandBuffer, lastMaterial);
        lastMaterial = &material;
    }

    //推送常量
    DefaultPushConstant pushConstant = {mul(matrixVP, modelMatrix)};
    glCommandBuffer.PushConstant(
        material.GetShader()->GetGLPipelineLayout(),
        material.GetShader()->GetPushConstantBinding()[0],
        &pushConstant);

    glCommandBuffer.DrawIndexed(mesh.GetGLIndexCount());
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
        VkRect2D{{0, 0}, {lastRenderTarget->GetWidth(), lastRenderTarget->GetHeight()}},
        color.has_value() ? std::optional(colorValue) : std::nullopt,
        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        depth.has_value() ? std::optional(depthStencilValue) : std::nullopt
    );
}
