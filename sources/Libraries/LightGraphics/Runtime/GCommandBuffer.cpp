#include "GCommandBuffer.h"

#include <cassert>

#include "LightMath/Runtime/MatrixMath.hpp"

using namespace Light;

void GCommandBuffer::BeginRecording()
{
    glCommandBuffer.BeginRecording();
}
void GCommandBuffer::EndRecording()
{
    if (currentRenderTarget != nullptr)
        EndRendering();

    glCommandBuffer.EndRecording();
    //全部属性重置为初始值
    currentMesh = nullptr;
    currentShader = nullptr;
    currentMaterial = nullptr;
    currentRenderTarget = nullptr;
}

void GCommandBuffer::BeginRendering(const GRenderTarget& renderTarget, const bool clearColor)
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
void GCommandBuffer::EndRendering()
{
    glCommandBuffer.EndRendering();

    //由于没有使用renderPass，因此布局变换得手动设置。每次渲染后图片布局默认为“未定义”，需在呈现前将布局调整为渲染目标期望的布局
    glCommandBuffer.TransitionImageLayout(
        currentRenderTarget->glFinalImage,
        VK_IMAGE_LAYOUT_UNDEFINED, currentRenderTarget->glFinalLayout,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    currentRenderTarget = nullptr;
}

void GCommandBuffer::SetRenderTarget(const GRenderTarget& renderTarget)
{
    if (currentRenderTarget != &renderTarget && currentRenderTarget != nullptr)
        EndRendering();
    BeginRendering(renderTarget);
}
void GCommandBuffer::SetRenderTargetToNull()
{
    EndRendering();
}

void GCommandBuffer::SetViewport(const int32_t x, const int32_t y, const uint32_t width, const uint32_t height)
{
    assert(x >=0 && static_cast<uint32_t>(x) < currentRenderTarget->width && "x超出最大渲染范围！");
    assert(y >=0 && static_cast<uint32_t>(y) < currentRenderTarget->height && "y超出最大渲染范围！");
    assert(x + width <= currentRenderTarget->width && "width超出最大渲染范围！");
    assert(y + height <= currentRenderTarget->height && "height超出最大渲染范围！");

    currentViewport = {
        VkOffset2D{x, y},
        VkExtent2D{width, height}
    };
    glCommandBuffer.SetViewport(
        static_cast<float>(x), static_cast<float>(y + height),
        static_cast<float>(width), -static_cast<float>(height)
    ); //利用-height的特性，实现剪辑空间的上下反转。这样传入左手坐标系的顶点后就可以负负得正了。
    glCommandBuffer.SetScissor(
        currentViewport.offset,
        currentViewport.extent
    );
}
void GCommandBuffer::SetViewportToFullscreen()
{
    SetViewport(0, 0, currentRenderTarget->width, currentRenderTarget->height);
}


void GCommandBuffer::DrawMesh(GMesh& mesh, GMaterial& material, const std::string_view& shaderPass)
{
    mesh.BindToPipeline(glCommandBuffer, currentMesh);
    material.BindToPipeline(glCommandBuffer, currentMaterial);
    currentMesh = &mesh;
    currentMaterial = &material;

    if (shaderPass.empty())
    {
        for (const auto& [passName,shader] : material.GetPasses())
        {
            shader->BindToPipeline(glCommandBuffer, currentShader);
            glCommandBuffer.DrawIndexed(mesh.GetGLIndexCount());
            currentShader = shader;
        }
    }
    else
    {
        for (const auto& [passName,shader] : material.GetPasses())
        {
            if (passName == shaderPass)
            {
                shader->BindToPipeline(glCommandBuffer, currentShader);
                glCommandBuffer.DrawIndexed(mesh.GetGLIndexCount());
                currentShader = shader;
            }
        }
    }
}
void GCommandBuffer::ClearRenderTarget(const std::optional<float4>& color, const std::optional<float>& depth) const
{
    assert(currentRenderTarget != nullptr && "渲染目标不能为空！");

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
        currentViewport,
        color.has_value() ? std::optional(colorValue) : std::nullopt,
        // ReSharper disable once CppLocalVariableMightNotBeInitialized
        depth.has_value() ? std::optional(depthStencilValue) : std::nullopt
    );
}