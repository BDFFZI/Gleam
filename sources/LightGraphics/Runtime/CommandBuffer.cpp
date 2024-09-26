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
void CommandBuffer::EndRecording()
{
    if (isRendering)
    {
        glCommandBuffer.EndRendering();
        isRendering = false;
    }

    glCommandBuffer.EndRecording();
}

void CommandBuffer::SetRenderTarget(const RenderTexture* renderTarget)
{
    if (isRendering)
    {
        glCommandBuffer.EndRendering();
        isRendering = false;
    }

    if (renderTarget != nullptr)
    {
        glCommandBuffer.BeginRendering(
            VkRect2D{
                {0, 0}, {
                    static_cast<uint32_t>(renderTarget->GetWidth()),
                    static_cast<uint32_t>(renderTarget->GetHeight())
                }
            },
            false, false,
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
            false, true,
            *Graphics::GetPresentColorImageView(),
            Graphics::GetPresentDepthStencilImageView().get(),
            Graphics::GetPresentColorResolveImageView().get()
        );
    }
    isRendering = true;
}
void CommandBuffer::SetViewport(const rect& viewport) const
{
    glCommandBuffer.SetViewportAndScissor(
        viewport.x, viewport.y,
        {static_cast<uint32_t>(viewport.width), static_cast<uint32_t>(viewport.height)}
    );
}
void CommandBuffer::ClearRenderTarget()
{
    
}
void CommandBuffer::Draw(const MeshBase& mesh, const Material& material) const
{
    glCommandBuffer.BindVertexBuffers(mesh.GetGLVertexBuffer());
    glCommandBuffer.BindIndexBuffer(mesh.GetGLIndexBuffer());

    const Shader& shader = material.GetShader();
    glCommandBuffer.BindPipeline(shader.GetGLPipeline());
    glCommandBuffer.PushDescriptorSet(shader.GetGLPipelineLayout(), material.GetDescriptorSet());

    glCommandBuffer.Draw(mesh.GetIndexCount());
}
