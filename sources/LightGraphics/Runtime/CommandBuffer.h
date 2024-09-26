#pragma once
#include "Material.h"
#include "Mesh.h"
#include "RenderTexture.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
#include "LightMath/Runtime/Graphics.h"

namespace LightRuntime
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
        CommandBuffer(const CommandBuffer&) = delete;

        GLCommandBuffer& GetGLCommandBuffer();

        void BeginRecording();
        void EndRecording();
        void SetRenderTarget(const RenderTexture* renderTarget);
        void SetViewport(const rect& viewport) const;
        void ClearRenderTarget();
        void Draw(const MeshBase& mesh, const Material& material) const;

    private:
        GLCommandBuffer glCommandBuffer;
        bool isRendering = false;
    };
}
