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
    glCommandBuffer.EndRecording();
    lastMesh = nullptr;
    lastMaterial = nullptr;
    lastShader = nullptr;
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

void CommandBuffer::SetViewport(float x, float y, float width, float height) const
{
    glCommandBuffer.SetViewport(
        x, y + height,
        width, -height
    ); //利用-height的特性，实现剪辑空间的上下反转。这样传入左手坐标系的顶点后就可以负负得正了。
    glCommandBuffer.SetScissor(
        {
            static_cast<int32_t>(x),
            static_cast<int32_t>(y)
        },
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        });
}
void CommandBuffer::PushConstant(const Shader& shader, const int slotIndex, void* data) const
{
    glCommandBuffer.PushConstant(
        shader.GetGLPipelineLayout(),
        shader.GetPushConstantBinding()[slotIndex],
        data);
}
void CommandBuffer::Draw(const MeshBase& mesh, const Material& material)
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
