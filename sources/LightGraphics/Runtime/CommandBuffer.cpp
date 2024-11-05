#include "CommandBuffer.h"

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
    lastShader = nullptr;
    lastRenderTexture = nullptr;
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
void CommandBuffer::PushConstant(const Shader& shader, const int slotIndex, void* data) const
{
    glCommandBuffer.PushConstant(
        shader.GetGLPipelineLayout(),
        shader.GetPushConstantBinding()[slotIndex],
        data);
}
void CommandBuffer::Draw(const Mesh& mesh, const Material& material)
{
    //绑定网格
    if (&mesh != lastMesh)
    {
        glCommandBuffer.BindVertexBuffers(mesh.GetGLVertexBuffer());
        glCommandBuffer.BindIndexBuffer(mesh.GetGLIndexBuffer());
        lastMesh = &mesh;
    }

    //绑定材质球
    if (&material != lastMaterial)
    {
        const Shader& shader = material.GetShader();
        glCommandBuffer.PushDescriptorSet(shader.GetGLPipelineLayout(), material.GetDescriptorSet());
        lastMaterial = &material;

        //绑定着色器
        if (&shader != lastShader)
        {
            glCommandBuffer.BindPipeline(shader.GetGLPipeline());
            lastShader = &shader;
        }
    }

    glCommandBuffer.DrawIndexed(mesh.GetIndexCount());
}
void CommandBuffer::ClearRenderTexture(float4 color, const float depth) const
{
    glCommandBuffer.ClearAttachments(
        VkRect2D{{0, 0}, {lastRenderTexture->GetWidth(), lastRenderTexture->GetHeight()}},
        color.data, depth, 0
    );
}
