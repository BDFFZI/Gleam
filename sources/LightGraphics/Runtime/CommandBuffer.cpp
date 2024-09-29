#include "CommandBuffer.h"

#include "Graphics.h"

using namespace LightRuntime;

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
    glCommandBuffer.SetViewportAndScissor(
        viewport.x, viewport.y,
        {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)}
    );
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

    glCommandBuffer.Draw(mesh.GetIndexCount());
}
