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
    //全部属性重置为初始值
    lastMesh = nullptr;
    lastMaterial = nullptr;
    lastRenderTexture = nullptr;
    matrixVP = float4x4::Identity();
}

void CommandBuffer::BeginRendering(const RenderTargetBase& renderTarget, const bool clearColor)
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

    lastRenderTexture = &renderTarget;
    SetViewportToFullscreen();
}
void CommandBuffer::EndRendering() const
{
    glCommandBuffer.EndRendering();
}

void CommandBuffer::SetViewport(const int32_t x, int32_t y, const uint32_t width, const uint32_t height) const
{
    y = static_cast<int32_t>(lastRenderTexture->GetHeight() - (y + height)); //转换到右手坐标系

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
void CommandBuffer::SetViewProjectionMatrices(const float4x4& matrixVP)
{
    this->matrixVP = matrixVP;
}

void CommandBuffer::SetViewportToFullscreen() const
{
    SetViewport(0, 0, lastRenderTexture->GetWidth(), lastRenderTexture->GetHeight());
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
        VkRect2D{{0, 0}, {lastRenderTexture->GetWidth(), lastRenderTexture->GetHeight()}},
        color.has_value() ? std::optional(colorValue) : std::nullopt,
        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        depth.has_value() ? std::optional(depthStencilValue) : std::nullopt
    );
}
