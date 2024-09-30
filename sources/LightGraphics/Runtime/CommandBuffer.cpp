#include "CommandBuffer.h"

#include "Graphics.h"

using namespace LightRuntime;

CommandBuffer::CommandBuffer(): glCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY)
{
}

GLCommandBuffer& CommandBuffer::GetGLCommandBuffer()
{
    return glCommandBuffer;
}
void CommandBuffer::BeginRecording()
{
    glCommandBuffer.BeginRecording();
}
void CommandBuffer::EndRecording() const
{
    glCommandBuffer.EndRecording();
}

void CommandBuffer::BeginRendering(const RenderTexture* renderTarget, const bool clearColor) const
{
    if (renderTarget != nullptr)
    {
        glCommandBuffer.BeginRendering(
            VkRect2D{
                {0, 0}, {
                    static_cast<uint32_t>(renderTarget->GetWidth()),
                    static_cast<uint32_t>(renderTarget->GetHeight())
                }
            }, clearColor,
            *renderTarget->GetGLColorImageView(),
            renderTarget->GetGLDepthStencilImageView().get(),
            renderTarget->GetGLColorResolveImageView().get()
        );
    }
    else
    {
        const std::unique_ptr<GLSwapChain>& glSwapChain = Graphics::GetGLSwapChain();


        glCommandBuffer.BeginRendering(
            VkRect2D{{0, 0}, glSwapChain->imageExtent},
            clearColor,
            *Graphics::GetPresentColorImageView(),
            Graphics::GetPresentDepthStencilImageView().get(),
            Graphics::GetPresentColorResolveImageView().get()
        );
    }
}
void CommandBuffer::EndRendering() const
{
    glCommandBuffer.EndRendering();
}

void CommandBuffer::SetViewport(const rect& viewport) const
{
    glCommandBuffer.SetViewport(
        viewport.x, viewport.y + viewport.height,
        viewport.width, -viewport.height
    ); //利用-height的特性，实现剪辑空间的上下反转。这样传入左手坐标系的顶点后就可以负负得正了。
    glCommandBuffer.SetScissor(
        {
            static_cast<int32_t>(viewport.x),
            static_cast<int32_t>(viewport.y)
        },
        {
            static_cast<uint32_t>(viewport.width),
            static_cast<uint32_t>(viewport.height)
        });
}
void CommandBuffer::Draw(MeshBase& mesh, const Material& material) const
{
    //绑定网格
    mesh.UpdateGLBuffer();
    glCommandBuffer.BindVertexBuffers(mesh.GetGLVertexBuffer());
    glCommandBuffer.BindIndexBuffer(mesh.GetGLIndexBuffer());
    //绑定着色器
    const Shader& shader = material.GetShader();
    glCommandBuffer.BindPipeline(shader.GetGLPipeline());
    //绑定标识符
    if (!material.GetDescriptorSet().empty())
        glCommandBuffer.PushDescriptorSet(shader.GetGLPipelineLayout(), material.GetDescriptorSet());

    glCommandBuffer.DrawIndexed(mesh.GetIndexCount());
}
